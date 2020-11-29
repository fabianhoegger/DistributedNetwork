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

#include "Node.h"
#include "MyPacket_m.h"

Define_Module(Node);

void Node::initialize()
{
    // TODO - Generated method body
    id=par("id");
    //ids.push_back(id);
    int size = gateSize("inoutGateVector");
    //neighbours.push_back(size);
    sink_neighbours=size;
    sink_id=id;
    //EV<<ids[0]<<endl;
    //EV<<neighbours[0]<<endl;
    if(id==0){

        for (int i=0;i< gateSize("inoutGateVector");i++){
            cMessage* msg =new cMessage("initial_flooding");
            send(msg->dup(),"inoutGateVector$o",i);
            //MyPacket *pkt = new MyPacket("pkt");
            //pkt->setSending_id(id);
            //pkt->setSending_neighbours(size);
            //send(pkt,"inoutGateVector$o",i);
            delete(msg);

               }
        state="send_id";
        cMessage* msg =new cMessage("send_id");
        scheduleAt(simTime()+1,msg);
   }
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    if(strcmp("initial_flooding",msg->getName())==0 && state=="Receiving") {
        int size = gateSize("inoutGateVector");
        cModule* sender= msg->getSenderModule();
        cModule*  current=msg->getArrivalModule();
        int  arrival_gate_index=msg->getArrivalGate()->getIndex();// sigkrino afto me to i gia na min stilo ston proigoumeno
        EV<<" arrival gate index for initial flooding is"<<arrival_gate_index<<endl;
        EV<<" current node  "<<current<<endl;
        EV<<"sender node is  "<<sender<<endl;
        delete(msg);
        for (int i=0;i<size;i++){
            cMessage* msg =new cMessage("initial_flooding");
            int tempgate=gate("inoutGateVector$o",i)->getIndex();
            if(tempgate== arrival_gate_index){
                EV<<"gate denied  "<<tempgate<<endl;

                continue;
            }
            send(msg->dup(),gate("inoutGateVector$o",i));
            EV<<"sent to gate  "<<tempgate<<endl;
            delete(msg);

        }
        state="send_id";
        cMessage* msg =new cMessage("send_id");
        scheduleAt(simTime()+1,msg);

        }
    else if(strcmp("send_id",msg->getName())==0 && state=="send_id") {
        int size = gateSize("inoutGateVector");
        MyPacket *pkt = new MyPacket("pkt");
        pkt->setSending_id(id);
        pkt->setSending_neighbours(size);
        for (int i=0;i< gateSize("inoutGateVector");i++){
            send(pkt->dup(),"inoutGateVector$o",i);
         }
        delete(pkt);
        state="Receiving2";
    }
    else if(strcmp("pkt",msg->getName())==0){
            if(state=="Sink_found"){
                delete(msg);
            }
            else if(state=="Receiving2"){
                MyPacket * pkt = dynamic_cast<MyPacket*> (msg);
                int received1,received2;
                received1=pkt->getSending_id();
                received2=pkt->getSending_neighbours();
                EV<<"ID RECEIVED :"<<received1<< " NEIGBOURS RECEIVED :"<<received2<<endl;
                int flag=0;//ELENXO AN EXO IDI TIN PLIROFORIA
                for(int i=0;i<ids.size();i++){
                    if(received1==ids[i]){
                        flag=1;
                    }
                }
                if(flag==0)
                {
                    ids.push_back(received1);// APOTHIKEUO
                    neighbours.push_back(received2);
                    int  arrival_gate_index=pkt->getArrivalGate()->getIndex();// sigkrino afto me to i gia na min stilo ston proigoumeno
                    EV<<" arrival gate index "<<arrival_gate_index<<endl;
                    int size = gateSize("inoutGateVector");

                    for (int i=0;i<size;i++){//STELNO PADOU EKTOS APO APOSTOLEA

                        int tempgate=gate("inoutGateVector$o",i)->getIndex();
                        if(tempgate== arrival_gate_index){
                            EV<<"gate denied  "<<tempgate<<endl;
                            continue;
                        }
                        send(pkt->dup(),gate("inoutGateVector$o",i));
                        EV<<"sent to gate  "<<tempgate<<endl;
                      }
                    delete(pkt);
                    for(int j=0;j<ids.size();j++){
                        if (received2>sink_neighbours){
                            sink_neighbours=received2;
                            sink_id=received1;
                         }
                        else if(received2==sink_neighbours && received1<sink_id){
                            sink_neighbours=received2;
                            sink_id=received1;
                        }
                        EV<<"  current sink ID  :"<<sink_id<< " current sink NEIGHBOURS  :"<<sink_neighbours<<endl;
                     }

                    if(ids.size()==9){

                        if(sink_id==id){
                            state="Sink_found";
                            int size = gateSize("inoutGateVector");
                            MyPacket *pkt = new MyPacket("Sink_found");
                            pkt->setSending_id(id);
                            pkt->setSending_neighbours(size);
                            pkt->setPos(0);
                            for (int j=0;j<size;j++){
                                pkt->setRouting(0,j);
                            }
                            for (int i=0;i<size;i++){//STELNO PADOU OTI EIMAI O SINK
                                send(pkt->dup(),gate("inoutGateVector$o",i));

                            }
                           delete(pkt);
                        }
                    }
                  }

                }


        }
    else if(strcmp("Sink_found",msg->getName())==0 ){
        if(state=="Sink_found"){
            delete(msg);
        }
        else if(state=="Receiving2"){
            state="Sink_found";
            int  arrival_gate_index=msg->getArrivalGate()->getIndex();// sigkrino afto me to i gia na min stilo ston proigoumeno
            MyPacket * pkt = dynamic_cast<MyPacket*> (msg);

            sink_id=pkt->getSending_id();
            sink_neighbours=pkt->getSending_neighbours();
            int size = gateSize("inoutGateVector");
            int pos=pkt->getPos();
            pos=pos+1;
            pkt->setPos(pos);
            EV<<"pos is  "<<pos<<endl;

            for(int i=0;i<size;i++){
                       pkt->setRouting(pos, i);
                       int value=pkt->getRouting(pos);
                       EV<<"array value  "<<pos<<"is"<<value <<endl;

                       int tempgate=gate("inoutGateVector$o",i)->getIndex();
                       if(tempgate== arrival_gate_index){
                           EV<<"gate denied  "<<tempgate<<endl;

                           continue;
                       }
                       send(pkt->dup(),gate("inoutGateVector$o",i));
                       EV<<"sent to gate  "<<tempgate<<endl;

                   }
            delete(pkt);
            cMessage* msg =new cMessage("Start_transmitting");
            scheduleAt(simTime()+1,msg);

        }
    }
    else if(strcmp("Start_transmitting",msg->getName())==0 ){
        if(state=="Start_transmitting"){
            delete(msg);
        }
        else if(state=="Sink_found"){
            state="Start_transmitting";
            delete(msg);
            MyPacket *pkt = new MyPacket("MINIMA");
            //pkt->setPos(1);
            //pkt->setRouting(1,1);
            pkt->setSending_id(id);

            int pos = pkt->getPos();
            pkt->setPos(pos-1);//miosi tou pos kata ena
            int gate =pkt->getRouting(pos);
            send(pkt,"inoutGateVector$o",gate);
            EV<<"sent to gate  "<<gate<<endl;
            delete(pkt);

        }
        else if(strcmp("MINIMA",msg->getName())==0 ){
           MyPacket * pkt = dynamic_cast<MyPacket*> (msg);
           int pos = pkt->getPos();
           pkt->setPos(pos-1);//miosi tou pos kata ena
           int gate =pkt->getRouting(pos);
           send(pkt,"inoutGateVector$o",gate);
           EV<<"sent to gate  "<<gate<<endl;
           delete(pkt);

                }
    }




}
void Node::finish()
{
    EV<<" sink ID  :"<<sink_id<< " sink NEIGBOURS  :"<<sink_neighbours<<endl;
    EV<<"IDS RECEIVED"<<ids.size()<<endl;
}

