//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

network Network
{
    parameters:
        @display("bgb=1,1");
        int n @prompt("Number of Nodes") = default(10);
        double rc= 0.4;
    submodules:
        node[n]: Node {
            //parameters:
                id=index;
                posX= uniform(0 , 1);
                posY= uniform(0 , 1);
                @display("p=$posX,$posY");
        }
    connections:
        for i=0..n-1,for j=i+1..n-1{
          node[i].inoutGateVector++ <--> node[j].inoutGateVector++
          if(sqrt(pow((node[i].posX-node[j].posX),2) +pow((node[i].posY-node[j].posY),2)) <= rc);
        }      
}
