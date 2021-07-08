#include "Unit_Mapping.h"

class Mine_Interface
{
        public:
                void new_depot(BWAPI::Unit nexus);
                void update_mine_interface(Unit_Mapping * map);
        private:

                struct mineral_timer
                {
                        BWAPI::Unit mineral; // mineral timer is assigned to
                        int num;         // timer number
                        int gather_time; // time it takes a worker to gather
                        int gatherers; // number of gatherers on mineral
                        int mining_time; // used for efficiency calculation
                };

                // collection of timer structures
                std::vector<mineral_timer> timers;

                bool halt_mining;

                // maps for worker status and assigned mineral
                std::map<BWAPI::Unit, bool> worker_gathering;
                std::map<BWAPI::Unit, bool> worker_queued;
                std::map<BWAPI::Unit, BWAPI::Unit> worker_mineral;

                void update_mines();
                void update_workers(BWAPI::Unit, Unit_Mapping *);
                void add_timer(BWAPI::Unit, BWAPI::Unit);
                mineral_timer * get_top_patch(BWAPI::Unit nexus, BWAPI::Unit unit);
                mineral_timer * get_timer(BWAPI::Unit);
                void percent_mining_time();
                bool get_worker(BWAPI::Unitset workers, BWAPI::Unit, BWAPI::Unit mineral);
                void remove_timers(Unit_Mapping * map);
                void update_idles(BWAPI::Unitset, Unit_Mapping * map);
                void worker_returning(BWAPI::Unit u);
                void assign_worker(BWAPI::Unit nexus, BWAPI::Unit u, BWAPI::Unitset workers);
                int ApproximateDistance(int x1, int x2, int y1, int y2);
                int Mine_Interface::EdgeToPointDistance(BWAPI::UnitType type, BWAPI::Position center, BWAPI::Position point);
                int Mine_Interface::generate_mineral_distance(BWAPI::Position center, BWAPI::Position center2);
                BWAPI::Position Mine_Interface::midpoint_on_closest_edge(BWAPI::Position point, BWAPI::UnitType type, BWAPI::Position center);

};

