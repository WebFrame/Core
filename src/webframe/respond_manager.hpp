#pragma once

#include <functional>
#include <webframe/base.hpp>

namespace webframe
{
struct responser
{
    public:
        responser ()
        {
        }

        std::function<response(const std::string&, const path_vars&)> call;

        template<typename Ret, typename... Ts>
        responser (std::function<Ret(Ts...)> f)
        {
            set (f, std::make_index_sequence<sizeof...(Ts)>{});
        }

        template<typename Ret>
        responser (std::function<Ret()> f)
        {
            call = [=](const std::string& http, __attribute__((unused)) const path_vars& vars) -> response {
                return response(http, f());
            };
        }

    private:
        template<typename Ret, typename... Ts, std::size_t... I>
        void set(std::function<Ret(Ts...)> f, __attribute__((unused)) std::index_sequence<I...> seq)
        {
            call = [=](const std::string& http, const path_vars& vars) -> response {
                return response(http, f((Ts(vars[I]))...));
            };
        }

        friend class webframe;
};
}