#include <memory>
#include <vector>
#include <iostream>
#include <atomic>
#include <functional>
#include <map>
#include <typeindex>
#include <chrono>

/*
 *  This files shows a simple example of how MARS object Processing Works
 *  Unlike MARS Engine this code is single threaded to prioritize the optimization of how layers are processed
 **/

class object;

class component : public std::enable_shared_from_this<component> {
private:
    std::weak_ptr<object> m_object;
public:
    explicit component(const std::weak_ptr<object>& _object) {
        m_object = _object;
    }

    virtual void destroy() { }
};

class object : public std::enable_shared_from_this<object> {
private:
    std::vector<std::shared_ptr<component>> m_components;
public:
    const std::vector<std::shared_ptr<component>>& components() const {
        return m_components;
    }

    template<typename T> std::weak_ptr<T> add_component(const std::shared_ptr<T>& _component) {
        m_components.push_back(_component);
        return _component;
    }

    template<typename T> std::weak_ptr<T> add_component() {
        return add_component<T>(std::make_shared<T>(shared_from_this()));
    }
};

struct engine_layer_component;

struct layer_component_param {
    engine_layer_component* component;
    std::shared_ptr<std::vector<engine_layer_component>> layers;
    size_t index;
};

struct engine_layer_component {
    void* target = nullptr;
    const object* parent;
    std::function<void(const layer_component_param&)> callback;
};

struct engine_layers {
    std::function<std::vector<engine_layer_component>(const object&)> m_validator;

    explicit engine_layers(const std::function<std::vector<engine_layer_component>(const object&)>& _validor) {
        m_validator = _validor;
    }
};

class engine {
private:
    std::vector<std::shared_ptr<object>> m_objects;
    std::vector<std::shared_ptr<object>> m_wait_objects;
    std::map<std::type_index, engine_layers> m_layer_data;
    std::map<std::type_index, std::shared_ptr<std::vector<engine_layer_component>>> m_layer_calls;
public:
    template<typename T> void add_layer(const std::function<std::vector<engine_layer_component>(const object&)>& _validator) {
        m_layer_data.insert(std::make_pair(std::type_index(typeid(T)), engine_layers(_validator)));
        m_layer_calls.insert(std::make_pair(std::type_index(typeid(T)), std::make_shared<std::vector<engine_layer_component>>()));
    }

    std::shared_ptr<object> create_obj() {
        auto obj = std::make_shared<object>();
        m_wait_objects.push_back(obj);
        return obj;
    }

    void spawn() {
        m_objects.resize(m_objects.size() + m_wait_objects.size());

        for(auto& object : m_wait_objects) {
            m_objects.push_back(object);

            for (auto& layer : m_layer_data) {
                std::vector<engine_layer_component> list = layer.second.m_validator(*object);

                if (list.empty())
                    continue;

                m_layer_calls[layer.first]->insert(m_layer_calls[layer.first]->end(), list.begin(), list.end());
            }
        }

        m_wait_objects.clear();
    }

    template<typename T> void process_layer() {
        auto type = std::type_index(typeid(T));

        for (auto& calls : m_layer_calls) {
            auto components = m_layer_calls[type];

            layer_component_param param {
                .layers = components,
            };

            for (size_t i = 0; i < components->size(); i++) {
                param.index = i;
                param.component = &components->at(i);
                components->at(i).callback(param);
            }
        }
    }
};

std::atomic<size_t> index = 0;

class update_layer {
public:
    virtual void update() { }
};

std::vector<engine_layer_component> update_layer_callback(const object& _target) {
    std::vector<engine_layer_component> list;

    for (auto& comp : _target.components()) {
        auto target = dynamic_cast<update_layer*>(comp.get());

        if (target == nullptr)
            continue;

        auto new_component = engine_layer_component();
        new_component.target = target;
        new_component.parent = &_target;
        new_component.callback = [](const layer_component_param& _param) {
            static_cast<update_layer*>(_param.component->target)->update();
        };

        list.push_back(new_component);
    }

    return list;
}

class test_update : public component, public update_layer {
public:
    using component::component;

    void update() override {
        index++;
    }
};

class tick {
private:
    std::chrono::_V2::system_clock::time_point m_last_tick = std::chrono::high_resolution_clock::now();
    float m_delta_time = 0.0001f;
    float m_delta_time_ms = 0.0001f;
public:
    [[nodiscard]] float delta() const { return m_delta_time; }
    [[nodiscard]] float delta_ms() const { return m_delta_time_ms; }

    void exec_tick() {
        auto now = std::chrono::high_resolution_clock::now();
        m_delta_time_ms = std::chrono::duration<float, std::chrono::milliseconds::period>(now - m_last_tick).count();
        m_delta_time = std::chrono::duration<float, std::chrono::seconds::period>(now - m_last_tick).count();
        m_last_tick = std::chrono::high_resolution_clock::now();
    }
};

int main() {
    engine _engine;
    _engine.add_layer<update_layer>(update_layer_callback);

    size_t l = 100'000;

    for (size_t i = 0; i < l; i++) {
        auto obj = _engine.create_obj();
        obj->add_component<test_update>();
    }

    _engine.spawn();

    tick t;
    float second = 0;
    while (true) {
        _engine.process_layer<update_layer>();
        t.exec_tick();

        if (index != l)
            throw "ERROR";
        index = 0;
        second += t.delta();
        if (second >= 1) {
            std::printf("TEST - %f\n", t.delta_ms());
            second = 0;
        }
    }
}