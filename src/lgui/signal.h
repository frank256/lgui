#ifndef LGUI_SIGNAL_H
#define LGUI_SIGNAL_H

#include <functional>
#include <list>
#include "platform/error.h"
#include <cstdint>

namespace lgui {
/** A template class providing a signal that slots can be connected to.
 * Use `connect` to connect a slot which is a std::function returning void.
 * To disconnect a specific slot again, save the return value of `connect` and pass it to `disconnect`.
 * Call `emit(...)` to emit the signal: all slots will be called. Arguments must match the signature of the
 * signal which is given as a template parameter.
 *
 * If a new slot is connected while emitting the signal is in progress, it will not be called in the same run.
 * If a slot is disconnected while emitting, it will not be called anymore if it would have been invoked after
 * the slot disconnecting it. However, it might have already been called before.
 *
 * You can chain signals by calling the overloaded `connect` method. Both signals must use the same signature.
 *
 * The signal will never know when slots, or rather objects referenced by slots, are destroyed. It's your job
 * to take care of that.
 *
 * Currently, you can only make ~ 1 billion total connections per signal before it will trigger an assert
 * and/or crash. This is not a problem for the use cases so far. It might, however, not be a good idea to
 * connect/reconnect in a tight loop.
 */

template<typename... Signature>
class Signal {
    public:
        using Slot = std::function <void (Signature...)>;
        using IdType = uint32_t;
        using Pair = std::pair <Slot, IdType>;
        using Container = std::list<Pair>;

        Signal() : mnext_id(1), mis_emitting(false), mneed_cleanup(false)
        {}

        Signal(const Signal& other) = delete;
        Signal operator=(const Signal& other) = delete;

        class DisconnectId {
                friend class Signal;
                explicit DisconnectId(IdType id)
                    : mid(id) {}
                IdType mid;
                public: // Can declare empty IDs.
                  DisconnectId() : mid(0) {}
        };

        DisconnectId connect(const Slot& slot) {
            ASSERT(mnext_id <= MAX_ID);
            IdType id = mnext_id++;
            mslots.emplace_back(Pair{slot, mis_emitting ? id | DONT_EMIT_YET : id});
            return DisconnectId(id);
        }

        DisconnectId connect(Slot &&slot) {
            ASSERT(mnext_id <= MAX_ID);
            IdType id = mnext_id++;
            mslots.emplace_back(Pair{std::move(slot), mis_emitting ? id | DONT_EMIT_YET : id});
            return DisconnectId(id);
        }

        /** Chains a signal to another signal. As always, be sure the other side is still alive. */
        template <typename... Signature2>
        DisconnectId connect(Signal <Signature2...>& other) {
            // Have to template this function so that perfect forwarding will work. Enforce at least the
            // number of arguments with a static assert to give a more helpful error message.
            static_assert(sizeof...(Signature) == sizeof...(Signature2),
                          "Expected the same number of arguments for signal chaining!");
            return connect([&other](Signature2...args) {
                other.emit(std::forward <Signature2> (args)...);
            });
        }

        /** Helper overload to bind method, non-const variant. */
        template<typename C, typename... Args>
        DisconnectId connect(void (C::* func)(Args...), C& instance) {
            return connect([func, &instance] (Args&&... args) {
                (instance.*func)(std::forward<Args>(args)...);
            });
        }

        /** Helper overload to bind method, const variant. */
        template<typename C, typename... Args>
        DisconnectId connect(void (C::* func)(Args...) const,
                             const C& instance) {
            return connect([func, &instance] (Args&&... args) {
                (instance.*func)(std::forward<Args>(args)...);
            });
        }

        /** Disconnect a slot from the signal. Will modify the id to become invalid. Will take
         * O(no_slots_connected) in order to check for validity. */
        bool disconnect(DisconnectId &id) {
            if(id.mid != 0) {
                ASSERT(id.mid <= MAX_ID);
                typename Container::iterator it;
                for(it = mslots.begin(); it != mslots.end(); ++it) {
                    if((it->second & ID_MASK) == id.mid)
                        break;
                }
                if(it == mslots.end()) {
                    // Has become invalid.
                    id.mid = 0;
                    return false;
                }
                if(mis_emitting) {
                    it->second |= TO_BE_REMOVED;
                    mneed_cleanup = true;
                }
                else
                    mslots.erase(it);
                // Can reclaim last ID issued.
                if(id.mid == mnext_id-1)
                    mnext_id--;
                id.mid = 0;
                return true;
            }
            else
                return false;
        }

        void disconnect_all() {
            if(!mslots.empty()) {
                if(mis_emitting) {
                    for(auto& s : mslots) {
                        s.second |= TO_BE_REMOVED;
                    }
                    mneed_cleanup = true;
                }
                else
                    mslots.clear();
            }
            // Now all DisconnectIds given out are invalid. :(
        }

        /** Use this function if you're sure you don't have any DisconnectIds lying around anymore.
         */
        void disconnect_all_no_regrets() {
            disconnect_all();
            mnext_id = 1; // Can reuse them.
        }

        /** Emit the signal. */
        template<typename... Args>
        void emit(Args &&... args)
        {
            if(mneed_cleanup)
                cleanup();
            if(!mis_emitting) {
                mis_emitting = true;
                for(auto it = mslots.begin(); it != mslots.end(); ++it) {
                    if(it->second & DONT_EMIT_YET)
                        it->second &= ~DONT_EMIT_YET; // Emit next time.
                    else if(!(it->second & TO_BE_REMOVED))
                        it->first(std::forward <Args> (args)...);
                    // Don't do anything if going to be removed.
                }
                mis_emitting = false;
            }
        }


    private:
        void cleanup() {
            if(mis_emitting)
                return;
            mslots.remove_if([](const Pair& p) {
                return p.second & TO_BE_REMOVED;
            });
            mneed_cleanup = false;
        }

        static const IdType TO_BE_REMOVED = 1u<<31u,
                            DONT_EMIT_YET = 1u<<30u,
                            MAX_ID = DONT_EMIT_YET-1u,
                            ID_MASK = MAX_ID;
        Container mslots;
        IdType mnext_id;
        bool mis_emitting, mneed_cleanup;
};

}

#endif // LGUI_SIGNAL_H
