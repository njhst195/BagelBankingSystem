#include <iostream>
#include <olc_net.h>

//enum class the same as the clients with the same constants for the switch statement
enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	CheckAccount,
	Deposit,
	Transfer,
	Login,
	Logout,
};

std::vector<uint32_t> validIDs; //vector for valid IDs

//server class
class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{

	}

	//functions the server can perform when given the proper header ID
protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;

		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& inMsg)
	{
		switch (inMsg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			for (int i = 0; i < validIDs.size(); i++)
			{
				std::cout << validIDs[i];
			}

			// Simply repeat message back to client
			client->Send(inMsg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: Message All\n";

			// Construct a new message and send it to all clients
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::ServerMessage;
			msg << client->GetID();
			MessageAllClients(msg, client);

		}
		break;

		case CustomMsgTypes::CheckAccount:
		{
			//client requested to check account
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::CheckAccount;
			
			uint32_t x = client->GetID();
			msg << x;

			std::cout << "ID #[" << client->GetID() << "]: Has accessed their account.\n";
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::Deposit:
		{
			//client requested to deposit
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Deposit;

			bool x = true;
			msg << x;

			std::cout << "ID #[" << client->GetID() << "]: Has been cleared to deposit to their account.\n";
			client->Send(msg);
		}
		break;


		case CustomMsgTypes::Transfer:
		{
			//client requested to transfer
			olc::net::message<CustomMsgTypes> msg;  
			msg.header.id = CustomMsgTypes::Transfer;

			bool x = true;
			msg << x;

			std::cout << "ID #[" << client->GetID() << "]: Has been cleared to transfer from savings to checking on their account.\n";
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::Login:
		{
			//client requested to login
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Login;

			//add to valid IDs
			validIDs.push_back(client->GetID());
			

			bool x = false;

			//check if client is in valid IDs
			for (int i = 0; i < validIDs.size(); i++)
			{
				if (client->GetID() == validIDs[i])
				{
					x = true;
					std::cout << "User #[" << client->GetID() << "] has logged in.\n";
				}
			}

			if (!x)
			{
				std::cout << "Failed login attempt.\n";
			}
			
			msg << x;
			client->Send(msg);
		}
		break;
		

		case CustomMsgTypes::Logout:
		{
			//client requested to logout
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Logout;

			std::cout << "User #[" << client->GetID() << "] has logged out.\n";

			client->Send(msg);
		}
		break;
		}
	}
};

//main server loop
int main()
{
	//defines server on port 60000
	CustomServer server(60000);
	server.Start();

	//sits in loop and updates, waiting for messages and connections
	while (1)
	{
		server.Update(-1, true);
	}

	return 0;
}