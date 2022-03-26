#pragma once

#include <functional>
#include <webframe/base.hpp>

namespace webframe
{

struct body_t {
    private:
    std::string body;
    public:
    body_t(std::string _body) : body(_body) {}
    const std::string& get_body() {
        return body;
    }
};

struct responser
{
    public:
        responser ()
        {
        }

        std::function<response(const std::string&, const std::string&, const path_vars&)> call;

        template<typename Ret, typename... Ts>
        responser (std::function<Ret(Ts...)> f)
        {
            set_without_body (f, std::make_index_sequence<sizeof...(Ts)>{});
        }

        template<typename Ret, typename... Ts>
        responser (std::function<Ret(body_t, Ts...)> f)
        {
            set_with_body (f, std::make_index_sequence<sizeof...(Ts)>{});
        }

        template<typename Ret>
        responser (std::function<Ret()> f)
        {
            call = [=](const std::string& http, __attribute__((unused)) const std::string& body, __attribute__((unused)) const path_vars& vars) -> response {
                return response(http, f());
            };
        }

        template<typename Ret>
        responser (std::function<Ret(body_t)> f)
        {
            call = [=](const std::string& http, const std::string& body, __attribute__((unused)) const path_vars& vars) -> response {
                return response(http, f(body));
            };
        }

    private:
        template<typename Ret, typename... Ts, std::size_t... I>
        void set_without_body(std::function<Ret(Ts...)> f, __attribute__((unused)) std::index_sequence<I...> seq)
        {
            call = [=](const std::string& http, __attribute__((unused)) const std::string& body, const path_vars& vars) -> response {
                return response(http, f((Ts(vars[I]))...));
            };
        }

        template<typename Ret, typename... Ts, std::size_t... I>
        void set_with_body(std::function<Ret(body_t, Ts...)> f, __attribute__((unused)) std::index_sequence<I...> seq)
        {
            call = [=](const std::string& http, const std::string& body, const path_vars& vars) -> response {
                return response(http, f(body, (Ts(vars[I]))...));
            };
        }

        friend class webframe;
};
}