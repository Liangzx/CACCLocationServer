#include "AMQPcpp.h"

using namespace std;

int i=0;

int onCancel(AMQPMessage * message );
int onMessage( AMQPMessage * message  );
void sendMsg2Mq();
void consumMsgFromMq();
void getMsg();

AMQP consum_amqp;

int main(int argc, char *argv[])
{
	sendMsg2Mq();

	
	consumMsgFromMq();

	//getMsg();

	while(1)
	{
		sleep(1);
	}

	return 0;
}

void sendMsg2Mq()
{
	try {
//		AMQP amqp;
//		AMQP amqp(AMQPDEBUG);
		/**
		 * description: connect string 
		 * user:password@host(主机ip或主机名):port/vhost(虚拟主机)
		 **/
		// vhost是/acdm，组装连接串时，是否需要在/前面增加一个/
		string strConn = "app:123456@172.18.57.208:5673//acdm";
		//AMQP amqp(strConn);		// all connect string
		AMQP amqp;
		amqp.ConnectToServer(strConn);

		cout << "send msg connect server success." << endl;

		AMQPExchange * ex = amqp.createExchange("e");
		ex->Declare("e", "fanout");

		AMQPQueue * qu2 = amqp.createQueue("q2");
		qu2->Declare();
		qu2->Bind( "e", "");		

		string ss = "message 1 ";
		/* test very long message
		ss = ss+ss+ss+ss+ss+ss+ss;
		ss += ss+ss+ss+ss+ss+ss+ss;
		ss += ss+ss+ss+ss+ss+ss+ss;
		ss += ss+ss+ss+ss+ss+ss+ss;
		ss += ss+ss+ss+ss+ss+ss+ss;
		*/

		ex->setHeader("Delivery-mode", 2);
		ex->setHeader("Content-type", "text/text");
		ex->setHeader("Content-encoding", "UTF-8");

		ex->Publish(  ss , ""); // publish very long message
		
		ex->Publish(  "message 2 " , "");
		ex->Publish(  "message 3 " , "");											
						
		cout << "send msg Publish finish." << endl;
	}catch (AMQPException e) {
		std::cout << e.getMessage() << std::endl;
	}
}

void consumMsgFromMq()
{
	try {
		string strConn = "app:123456@172.18.57.208:5673//acdm";
		consum_amqp.ConnectToServer(strConn);

		cout << "consum msg connect server success." << endl;

		AMQPQueue * qu2 = consum_amqp.createQueue("q2");

		qu2->Declare();
		qu2->Bind( "e", "");

		qu2->setConsumerTag("tag_123");
		qu2->addEvent(AMQP_MESSAGE, onMessage );
		qu2->addEvent(AMQP_CANCEL, onCancel );

		qu2->Consume(AMQP_NOACK);

		cout << "consum msg register process func finish." << endl;
	} catch (AMQPException e) {
		std::cout << e.getMessage() << std::endl;
	}
}

int onCancel(AMQPMessage * message ) {
	cout << "cancel tag="<< message->getDeliveryTag() << endl;
	return 0;
}

int onMessage( AMQPMessage * message  ) {
	uint32_t j = 0;
	char * data = message->getMessage(&j);
	if (data){
		cout << data << endl;

		i++;

		cout << "#" << i << " tag="<< message->getDeliveryTag() << " content-type:"<< message->getHeader("Content-type") ;
		cout << " encoding:"<< message->getHeader("Content-encoding")<< " mode="<<message->getHeader("Delivery-mode")<<endl;

		if (i > 10) {
			AMQPQueue * q = message->getQueue();
			q->Cancel( message->getConsumerTag() );
		}
	}

	return 0;
}

void getMsg()
{
	try {
		string strConn = "app:123456@172.18.57.208:5673//acdm";
		AMQP amqp;
		amqp.ConnectToServer(strConn);	

		cout << "get msg connect server success." << endl;

		AMQPQueue * qu2 = amqp.createQueue("q2");
		qu2->Declare();		


		while (  1 ) {
			qu2->Get(AMQP_NOACK);

			AMQPMessage * m= qu2->getMessage();

			cout << "count: "<<  m->getMessageCount() << endl;											 
			if (m->getMessageCount() > -1) {
				uint32_t j = 0;
				cout << "message\n"<< m->getMessage(&j) << "\nmessage key: "<<  m->getRoutingKey() << endl;
				cout << "exchange: "<<  m->getExchange() << endl;											
				cout << "Content-type: "<< m->getHeader("Content-type") << endl;	
				cout << "Content-encoding: "<< m->getHeader("Content-encoding") << endl;	
			} else {
				break;				
			}
		}	
	} catch (AMQPException e) {
		std::cout << e.getMessage() << std::endl;
	}
}
