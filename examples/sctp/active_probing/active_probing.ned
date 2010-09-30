//
// Copyright (C) 2010 Alberto Cortés Martín
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package inet.examples.sctp.active_probing;

import ned.DatarateChannel;
import inet.nodes.inet.StandardHost;
import inet.nodes.inet.Router;
import inet.world.ScenarioManager;

@license(LGPL);

network ap
{
    parameters:
        @display("bgl=3;bgb=750,400");

    types:
        channel channel_copper_wire extends ned.DatarateChannel {
            double distance @unit(m);
             // propagation delay: 5 ns for each meter
             // 200,000km/s speed of light in copper wire
             //    1m  -> 5ns
             //   10m  -> 50ns
             //  100m  -> 0.5us
             //    1km -> 5us
             //   10km -> 50us
             //  100km -> 0.5ms
             // 1000km -> 5ms
            delay = this.distance / 200000km * 1s;
        }
        channel channel_slow extends channel_copper_wire {
            // propagation delay of 200Km of copper = 1ms
            distance = 200km;
            disabled = false;
            // transmission time of 100Kbps with 536B + 40B +7B = 583B = 4664b = 46.64ms
            datarate = 100Kbps;
            ber = 0;
            per = 0;
        }
        channel channel_fast extends channel_copper_wire {
            // propagation delay of 5m of cooper = 25ns
            distance = 5m;
            disabled = false;
            // transmission time at 10Mbps of 536B + 40B + 7B = 583B = 4664b : 46.64ms
            datarate = 10Mbps;
            ber = 0;
            per = 0;
        }

    submodules:

        scenarioManager: ScenarioManager {
            @display("p=600,200");
        }

        source_noap: StandardHost {
            @display("p=100,100;i=device/server");
            gates:
                pppg[2];
        }

        sink_noap: StandardHost {
            @display("p=550,100;i=device/laptop");
            gates:
                pppg[2];
        }

        source_ap: StandardHost {
            @display("p=100,300;i=device/server");
            gates:
                pppg[2];
        }

        sink_ap: StandardHost {
            @display("p=550,300;i=device/laptop");
            gates:
                pppg[2];
        }

        router_1a: Router {
            @display("p=250,50;i=abstract/router");
            gates:
                pppg[2];
        }

        router_1z: Router {
            @display("p=400,50;i=abstract/router");
            gates:
                pppg[2];
        }

        router_2a: Router {
            @display("p=250,150;i=abstract/router");
            gates:
                pppg[2];
        }

        router_2z: Router {
            @display("p=400,150;i=abstract/router");
            gates:
                pppg[2];
        }

        router_3a: Router {
            @display("p=250,250;i=abstract/router");
            gates:
                pppg[2];
        }

        router_3z: Router {
            @display("p=400,250;i=abstract/router");
            gates:
                pppg[2];
        }

        router_4a: Router {
            @display("p=250,350;i=abstract/router");
            gates:
                pppg[2];
        }

        router_4z: Router {
            @display("p=400,350;i=abstract/router");
            gates:
                pppg[2];
        }



    connections:
        // RTT(ms) ~ 100ms (536B) to go ~ 50ms
        // RTT (ms) ~ 10ms (40B) to go ~ 5ms
        source_noap.pppg[0] <--> channel_fast <--> router_1a.pppg[0];
        router_1a.pppg[1]   <--> channel_slow <--> router_1z.pppg[0];
        router_1z.pppg[1]   <--> channel_fast <--> sink_noap.pppg[0];
        source_noap.pppg[1] <--> channel_fast <--> router_2a.pppg[0];
        router_2a.pppg[1]   <--> channel_slow <--> router_2z.pppg[0];
        router_2z.pppg[1]   <--> channel_fast <--> sink_noap.pppg[1];

        source_ap.pppg[0] <--> channel_fast <--> router_3a.pppg[0];
        router_3a.pppg[1] <--> channel_slow <--> router_3z.pppg[0];
        router_3z.pppg[1] <--> channel_fast <--> sink_ap.pppg[0];
        source_ap.pppg[1] <--> channel_fast <--> router_4a.pppg[0];
        router_4a.pppg[1] <--> channel_slow <--> router_4z.pppg[0];
        router_4z.pppg[1] <--> channel_fast <--> sink_ap.pppg[1];
}
