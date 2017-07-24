#pragma once

#include <memory>
#include <unordered_map>

namespace observable {

/* /brief Aquisição do manipulador de um objeto.
 *
 * Manipuladores são responsáveis por disponibilizarem os sinais de
 * notificação de alteração do estado de um objeto, como também métodos
 * de acesso que possam garantir a invariante de que qualquer
 * alteração no estado deve gerar a emissão de um sinal. A leitura do
 * estado é possível através de uma referência lvalue constante
 * retornada pelo método model().
 *
 * Um manipulador permite que múltiplas visões de um modelo recebam
 * notificações de alteração do estado do objeto. Para cada objeto de
 * um modelo há um único manipulador. O ciclo de vida do manipulador
 * depende do ciclo de vida do objeto. É um comportamento indefinido
 * utilizar um manipulador referente a um objeto já destruído. 
 *
 * Como usar:
 * 0. Implementar um manipulador provendo sinais e setters. Atender aos 
 *    requisitos descritos para Manipulator.
 * 1. O manipulador de um objeto é adquirido através da function template
 *    std::shared_ptr<Manipulator> manipulator(Model&). 
 * 2. As visões interessadas em ler e/ou alterar um objeto devem obter um 
 *    std::shared_ptr<Manipulator> referente ao objeto de interesse.
 * 3. Uma visão interessada em leitura deve obter o objeto através do método 
 *    de acesso model():
 *      auto& model = manipulator->model();
 * 4. Uma visão interessada em receber notificações de alteração de um objeto
 *    deve conectar um slot no sinal de interesse que é provido pelo 
 *    manipulador, por exemplo:
 *      manipulator->nameChanged.connect([](std::string oldName){...});
 * 5. Uma visão interessada em alterar o estado de um objeto deve utilizar a 
 *    interface de setters do manipulador, por exemplo:
 *      manipulator->name("newName");
 * 
 * Exemplo:
 * struct MarkerManipulator {
 *   using Model = Marker;
 *   MarkerManipulator(Marker& m) : m(m) {}
 *   const Marker& model(){ return m; }
 *   void name(std::string name) { 
 *     auto oldName = m.name;
 *     m.name = name; 
 *     nameChanged(oldName); 
 *   }
 *   boost::signals2::signal<void(std::string)> nameChanged;
 * private:    
 *   Marker& m;
 * };
 * 
 * auto manip = manipulator<MarkerManipulator>(marker);
 *
 * /tparam Manipulator Manipulador a ser implementado. Requisitos:
 *         1. Atributo Model com o tipo do modelo(Model);
 *         2. Método de acesso para obter uma referência lvalue constante para 
 *            Model: const Model& model();n
 * /param Model Referência para um objeto do modelo a ser manipulado.
 * /return std::shared_ptr para Manipulator.
 */    
template<typename Observable>
inline std::shared_ptr<Observable> from(typename Observable::Model& m)
{
    static std::unordered_map<typename Observable::Model*,
                              std::weak_ptr<Observable>> model2Observable;
    auto observable = model2Observable[&m].lock();
    if (!observable)
    {
        auto& lmodel2Observable = model2Observable;
        observable = std::shared_ptr<Observable>
            (new Observable(m),
             [&lmodel2Observable, &m](Observable* p)
             {
                 lmodel2Observable.erase(&m);
                 delete p;
             });
        model2Observable[&m] = observable;
    }
    return observable;
}

}
