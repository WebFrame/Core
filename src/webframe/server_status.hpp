/** 
 *  @file   server_status.hpp 
 *  @brief  The class will manage the status of the server ran on each port
 *  @author Alex Tsvetanov
 *  @date   2022-03-07
 ***********************************************/

#pragma once
#include <webframe/constexpr.hpp>
#include <shared_mutex>

namespace webframe {
	class server_status {
	public:
        server_status() {

        }
    private:
		std::mutex start [65536];
		std::mutex dead  [65536];
		
    public:
		void initiate(const char* PORT) {
			this->lock_working(PORT);
			this->lock_dead(PORT);
		}

        void alert_start(const char* PORT) {
            this->unlock_working(PORT);
        }

        void alert_end(const char* PORT) {
            this->unlock_dead(PORT);
        }

        std::mutex& get_start(const char* PORT) {
            return this->start[_compile_time::string_to_uint(PORT)];
        }

        std::mutex& get_end(const char* PORT) {
            return this->dead[_compile_time::string_to_uint(PORT)];
        }

    private:
		void lock_working(const char* PORT) {
			this->start[_compile_time::string_to_uint(PORT)].lock();
		}

		void unlock_working(const char* PORT) {
			this->start[_compile_time::string_to_uint(PORT)].unlock();
		}

		void lock_dead(const char* PORT) {
			this->dead[_compile_time::string_to_uint(PORT)].lock();
		}

		void unlock_dead(const char* PORT) {
			this->dead[_compile_time::string_to_uint(PORT)].unlock();
		}
	};
}