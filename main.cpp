#include <memory>
#include <vector>
#include <iostream>
#include <atomic>
#include <functional>
#include <map>
#include <typeindex>
#include <chrono>
#include <deque>

/*
 *  This files shows a simple example of how MARS object Processing Works
 *  Unlike MARS Engine this code is single threaded to prioritize the optimization of how layers are processed
 **/

class object;
class engine;

class component : public std::enable_shared_from_this<component> {
private:
    std::weak_ptr<object> m_object;
public:
    std::weak_ptr<object> get_object() const {
        return m_object;
    }

    explicit component(const std::weak_ptr<object>& _object) {
        m_object = _object;
    }

    virtual void destroy() { }
};

class object : public std::enable_shared_from_this<object> {
private:
    std::vector<std::shared_ptr<component>> m_components;
    std::weak_ptr<engine> m_engine;
public:
    explicit object(const std::weak_ptr<engine>& _weak) {
        m_engine = _weak;
    }

    const std::vector<std::shared_ptr<component>>& components() const {
        return m_components;
    }

    template<typename T> std::weak_ptr<T> add_component(const std::shared_ptr<T>& _component);

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
    std::weak_ptr<object> parent;
};

struct engine_layers {
    void (*m_callback)(const layer_component_param&);
    std::function<bool(const std::weak_ptr<component>&, engine_layer_component&)> m_validator;

    explicit engine_layers(const std::function<bool(const std::weak_ptr<component>&, engine_layer_component&)>& _validor, void (*_callback)(const layer_component_param&)) {
        m_validator = _validor;
        m_callback = _callback;
    }
};

class engine : public std::enable_shared_from_this<engine> {
private:
    std::deque<std::shared_ptr<object>> m_objects;
    std::map<std::type_index, engine_layers> m_layer_data;

    std::map<std::type_index, std::shared_ptr<std::vector<engine_layer_component>>> m_wait_list;
    std::map<std::type_index, std::shared_ptr<std::vector<engine_layer_component>>> m_layer_calls;
public:
    template<typename T> void add_layer(const std::function<bool(const std::weak_ptr<component>&, engine_layer_component&)>& _validator, void (*_callback)(const layer_component_param&)) {
        m_layer_data.insert(std::make_pair(std::type_index(typeid(T)), engine_layers(_validator, _callback)));
        m_layer_calls.insert(std::make_pair(std::type_index(typeid(T)), std::make_shared<std::vector<engine_layer_component>>()));
        m_wait_list.insert(std::make_pair(std::type_index(typeid(T)), std::make_shared<std::vector<engine_layer_component>>()));
    }

    std::weak_ptr<object> create_obj() {
        auto obj = std::make_shared<object>(shared_from_this());
        m_objects.push_back(obj);
        return obj;
    }

    void spawn() {
        for (auto& layer : m_layer_data) {
            if (m_wait_list.at(layer.first)->empty())
                continue;

            m_layer_calls.at(layer.first)->insert(m_layer_calls.at(layer.first)->end(), m_wait_list.at(layer.first)->begin(), m_wait_list.at(layer.first)->end());
            m_wait_list.at(layer.first)->clear();
        }
    }

    void process_component(const std::weak_ptr<component>& _component) {
        for (auto& layer : m_layer_data) {
            engine_layer_component val;
            if (m_layer_data.at(layer.first).m_validator(_component, val))
                m_wait_list.at(layer.first)->push_back(val);
        }
    }

    template<typename T> void process_layer() {
        auto type = std::type_index(typeid(T));
        void (*callback)(const layer_component_param&) = m_layer_data.at(type).m_callback;
        auto components = m_layer_calls[type];

        layer_component_param param {
                .layers = components,
        };

        for (size_t i = 0; i < components->size(); i++) {
            param.index = i;
            param.component = &components->at(i);
            callback(param);
        }
    }

    void destroy(const std::weak_ptr<object>& _obj) {
        for (auto& layer : m_layer_data) {
            m_layer_calls.at(layer.first)->erase(std::remove_if(m_layer_calls.at(layer.first)->begin(), m_layer_calls.at(layer.first)->end(), [&](const engine_layer_component& val) {
                return val.parent.lock() == _obj.lock();
            }), m_layer_calls.at(layer.first)->end());
            m_wait_list.at(layer.first)->erase(std::remove_if(m_wait_list.at(layer.first)->begin(), m_wait_list.at(layer.first)->end(), [&](const engine_layer_component& val) {
                return val.parent.lock() == _obj.lock();
            }), m_wait_list.at(layer.first)->end());
        }

        m_objects.erase(std::find(m_objects.begin(), m_objects.end(), _obj.lock()));
    }
};

template<typename T> std::weak_ptr<T> object::add_component(const std::shared_ptr<T> &_component) {
    m_components.push_back(_component);

    auto ref = std::weak_ptr<T>(_component);

    m_engine.lock().get()->process_component(ref);

    return ref;
}

std::atomic<size_t> global_index = 0;

class update_layer {
public:
    virtual void update() { }
};

void update_layer_callback(const layer_component_param& _param) {
    static_cast<update_layer*>(_param.component->target)->update();
}

bool update_layer_validator(const std::weak_ptr<component>& _target, engine_layer_component& _val) {

    auto target = dynamic_cast<update_layer*>(_target.lock().get());

    if (target == nullptr)
        return false;

    _val.target = target;
    _val.parent = _target.lock()->get_object();

    return true;
}

class test_update : public component, public update_layer {
public:
    using component::component;

    void update() override {
        global_index++;
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

class func_inc {
private:
    std::atomic<size_t> temp = 0;
public:
    void inc() volatile  {
        temp++;
    }
};

int main() {
    auto _engine = std::make_shared<engine>();
    _engine->add_layer<update_layer>(update_layer_validator, update_layer_callback);

    size_t l = 100'000;

    for (size_t i = 0; i < l; i++) {
        auto obj = _engine->create_obj();
        obj.lock()->add_component<test_update>();
    }

    _engine->spawn();

    //calc base time;

    tick t;
    std::atomic<size_t> temp = 0;
    t.exec_tick();
    for (size_t i = 0; i < l; i++)
        temp++;
    t.exec_tick();
    std::printf("Base time - %f\n", t.delta_ms());

    func_inc fi;

    t.exec_tick();
    for (size_t i = 0; i < l; i++)
        fi.inc();
    t.exec_tick();

    std::printf("Base Func time - %f\n", t.delta_ms());

    float second = 0;

    float time_since_start = 0.0f;

    while (time_since_start < 5) {
        t.exec_tick();
        _engine->process_layer<update_layer>();
        _engine->spawn();
        t.exec_tick();

        if (global_index != l)
            throw "ERROR";

        global_index = 0;
        second += t.delta();
        if (second >= 1) {
            std::printf("Tick time - %f\n", t.delta_ms());
            second = 0;
        }

        time_since_start += t.delta();
    }

    time_since_start = 0.0f;

    auto obj = _engine->create_obj();
    obj.lock()->add_component<test_update>();
    _engine->spawn();

    while (time_since_start < 5) {
        t.exec_tick();
        _engine->process_layer<update_layer>();
        _engine->destroy(obj);
        obj = _engine->create_obj();
        obj.lock()->add_component<test_update>();
        _engine->spawn();
        t.exec_tick();

        if (global_index != l + 1)
            throw "ERROR";

        global_index = 0;
        second += t.delta();
        if (second >= 1) {
            std::printf("Reallocate Tick time - %f\n", t.delta_ms());
            second = 0;
        }

        time_since_start += t.delta();
    }
}