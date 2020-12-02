#include <iostream>
#include <olc_net.h>

//this is an enum class list of constant things that can happen for the switch statement
//these are the headaers of all the messages for the system to identify what it needs to do
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
//class for the client
//decided to have it client side, and have the permission to do anything be validated by the server
class account
{
private:
	double savings;
	double checking;
	uint32_t ID;
public://values for each account 
	account() // constructor 
	{
		this->ID = NULL;
		this->savings = 0.0;
		this->checking = 0.0;
	}
//functions needed for the banking system
public:
	void Deposit(double d)
	{
		savings += d;
		std::cout << "New Savings Balance: " << savings << "\n";
	}
	void Transfer(double w)
	{
		savings -= w;
		checking += w;
		std::cout << "New Savings Balance: " << savings << "\n";
		std::cout << "New Checking Balance: " << checking << "\n";
	}
	double getBalance()
	{
		return savings;
	}
	void setBalance(double b)
	{
		this->savings = b;
	}
	uint32_t getID()
	{
		return ID;
	}
	void setID(uint32_t id)
	{
		this->ID = id;
	}
	double getChecking()
	{
		return checking;
	}
	void setChecking(double c)
	{
		this->checking = c;
	}
};

//class of functions the client can perform
//messages have header IDs and then the actual body of data
//the header needs to be set so the server knows what to do with a message by reading the header
class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		Send(msg);
	}
	
	void CheckAccount()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::CheckAccount;
		Send(msg);
	}

	void Deposit(double dep)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Deposit;

		msg << dep;
		Send(msg);
	}

	void Transfer(double tran)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Transfer;

		msg << tran;
		Send(msg);
	}

	void Login(uint32_t id)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Login;

		msg << id;
		Send(msg);
	}

	void Logout()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Logout;
		
		Send(msg);
	}
};

void menu() //menu display function
{
	std::cout << "Welcome to the Bagel Banking System (BBS)! \n Here are the options:\n";
	std::cout << "l. To login\no. To logout\na. Ping the Server\ns. Message All\nd. Disconnect\nf. Check Account\ng. Deposit\nr. Transfer\n\n";
}

int main()
{
	//create a client object
	CustomClient c;
	//connect to local ip and socket
	c.Connect("127.0.0.1", 60000);
	std::cout << "The Server will connect shortly...\n";

	account act; //new user account

	//keys and old keys for asynchronos key presses in the menus
	bool key[8] = { false, false, false, false, false, false, false, false };
	bool old_key[8] = { false, false, false, false, false, false, false, false };

	//used for inputs 
	bool loginFlag = false;
	double dep = 0.0;
	double with = 0.0;
	//print out menu
	menu();

	//main while loop for the client
	bool bQuit = false;
	while (!bQuit)
	{
		//binding the keys to their hexadecimal values
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState(0x41) & 0x8000;
			key[1] = GetAsyncKeyState(0x53) & 0x8000;
			key[2] = GetAsyncKeyState(0x44) & 0x8000;
			key[3] = GetAsyncKeyState(0x46) & 0x8000;
			key[4] = GetAsyncKeyState(0x47) & 0x8000;
			key[5] = GetAsyncKeyState(0x52) & 0x8000;
			key[6] = GetAsyncKeyState(0x4C) & 0x8000;
			key[7] = GetAsyncKeyState(0x4F) & 0x8000;
		}

		//each key press has different checks and calls different functions
		//this is how users interact
		if (key[0] && !old_key[0]) c.PingServer(); //ping server
		if (key[1] && !old_key[1]) c.MessageAll(); //message all cient with a preset message
		if (key[2] && !old_key[2]) bQuit = true; //disconnect

		if (key[3] && !old_key[3]) //check account
		{
			if (loginFlag)
				c.CheckAccount();
			else
				std::cout << "Please login first.\n";
		}

		if (key[4] && !old_key[4]) //deposit
		{
			if (loginFlag)
			{
				int checkNum;
				dep = 0;
				double x = 0;
		
				while (true)
				{
					std::cout << "Enter Deposit: ";
					std::cin >> x;

					std::cout << "Enter check number: ";
					std::cin >> checkNum;
					if (x > 0)
					{
						break;
					}
					else
						std::cout << "Invalid deposit input. try again\n";
				}

				dep += x;
				c.Deposit(x);
			}
			else
			{
				std::cout << "You are not logged in, press 'l' to login.\n";
			}
		}
		if (key[5] && !old_key[5]) //transfer
		{
			if (loginFlag)
			{
				with = 0;
				double x = 0;
				std::cout << "Enter Transfer amount: ";
				std::cin >> x;

				if (x > act.getBalance())
				{
					std::cout << "You cannot transfer more than you have in your savings\n";
				}
				else

				{
					with += x;
					c.Transfer(x);
				}
			}
			else
			{
				std::cout << "You are not logged in, press 'l' to login.\n";
			}
		}
		if (key[6] && !old_key[6]) //login
		{
			uint32_t tID;
			std::cout << "Enter ID: ";
			std::cin >> tID;

			c.Login(tID);
		}
		if (key[7] && !old_key[7]) //logout
		{
			c.Logout();
			system("PAUSE");
		}

		for (int i = 0; i < 7; i++) //while loop to check for key presses
		{
			old_key[i] = key[i];
		}

		if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{
				//get teh first set of bytes in the list of messages from the server

				auto msg = c.Incoming().pop_front().msg;
				
				//switch statement for the header ID 
				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted Connection\n";

				}
				break;


				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request	
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from another BBS user!\n";
				}
				break;

				case CustomMsgTypes::CheckAccount:
				{
					//checking account 
					std::cout << "Accessing your account\n";
					uint32_t ID;
					msg >> ID;
					std::cout << "Your ID is: " << ID << ", and the balance on that account ID is: " << act.getBalance() << "\n";
				}
				break;

				case CustomMsgTypes::Deposit:
				{
					//depositing 
					bool flag;
					msg >> flag;
					if (flag && loginFlag)
					{
						std::cout << "You have been cleared to deposit " << dep << " dollars into your account.\n";
						act.Deposit(dep);
					}
					else
					{
						std::cout << "You did not get proper permission to alter this account.\n";
					}
				}
				break;

				case CustomMsgTypes::Transfer:
				{
					//transfering money
					bool flag; 
					msg >> flag;
					if (flag && loginFlag)
					{
						std::cout << "You have been cleared to transfer " << with << " dollars from from your savings to your checkings account.\n";
						act.Transfer(with);
					}
					else
					{
						std::cout << "You did not get proper permission to alter this account.\n";
					}
				}
				break;

				case CustomMsgTypes::Login:
				{
					//logging in
					bool flag;
					msg >> flag;
					if (flag)
					{
						std::cout << "Login Successful...\n";
						loginFlag = true;
					}
					else
					{
						std::cout << "Login Failed.\n";
					}
				}
				break;

				case CustomMsgTypes::Logout:
				{
					//logging out
					std::cout << "Logout Successful...\n";
					loginFlag = false;
				}
				break;

				}
			}
		}
		else
		{ //cleanup
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}
	//end
	return 0;
}