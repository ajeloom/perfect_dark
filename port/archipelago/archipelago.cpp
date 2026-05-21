#include "archipelago.h"

#include "apclient.hpp"
#include "apuuid.hpp"
#include "defaultdatapackagestore.hpp"

extern "C" {
    #include "itemhandler.h"
}

using nlohmann::json;

#ifdef __EMSCRIPTEN__
#define DATAPACKAGE_CACHE "/settings/datapackage.json"
#define UUID_FILE "/settings/uuid"
#else
#define DATAPACKAGE_CACHE "datapackage.json"
#define UUID_FILE "uuid"
#define CERT_STORE "cacert.pem"
#endif

bool ap_sync_queued = false;
APClient* ap;

#define VERSION_TUPLE {0, 6, 5}

std::string URI;
std::string slotName;
std::string password;
std::string status = "Not connected\n";

int nextCheckToGet = 0;

int completionGoal;

extern int missionStars;
int requiredMissionStars;
extern int cachedMissionStars;

extern int progressiveWeapon;
int weaponProgressionType;
int allowProgWeaponInChallenges;

int hasChallenges;
int hasWeaponTraining;
int hasDeviceTraining;
int hasHolotraining;

int deathLink;
bool pendingDeathLink;

void AP_Init()
{
    AllocConsole();
	SetConsoleTitleA("Perfect Dark - Archipelago Console");

    FILE *fp;
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);

    system("cls");

    printf("--------------------------------------------------------------------------------------------------\n");
    printf("Commands:\n");
    printf(" - '/help' - shows available commands\n");
	printf(" - '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' - to connect to the room\n");
    printf(" - '/disconnect' - to exit the room\n");
	printf("--------------------------------------------------------------------------------------------------\n");

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InputCommand, NULL, 0, NULL);
}

void AP_Close() {
    FreeConsole();

    if (ap != nullptr) {
        delete ap;
        ap = nullptr;
    }
}

VOID InputCommand()
{
    while (true) {
        std::string line;
		std::getline(std::cin, line);
        
        if (line == "/help") {
            printf("--------------------------------------------------------------------------------------------------\n");
			printf("Commands: \n");
            printf(" - '/help' - shows available commands\n");
            if (ap) {
                printf(" - '!help' - Prints the help message related to Archipelago.\n");
            }
            printf(" - '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' - to connect to the room\n");
            printf(" - '/disconnect' - to exit the room\n");
            printf("--------------------------------------------------------------------------------------------------\n");
		}
        else if (line[0] == '!') {
            Message(line);
        }
        else if (line.find("/connect ") == 0) {
            
			std::string param = line.substr(9);
			int spaceIndex = param.find(" ");

			if (spaceIndex == -1) {
                printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
			} else {
                URI = param.substr(0, spaceIndex);
                if (spaceIndex == 0) {
                    printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
                    continue;
                }

                int passwordIndex = param.find("password:");
				std::string password = "";
				if (passwordIndex != -1) {
					password = param.substr(passwordIndex + 9);
				}

				slotName = param.substr(spaceIndex + 1, passwordIndex - spaceIndex - 2);
				
                std::cout << "URI: " << URI << "\n";
                std::cout << "slotname: " << slotName << "\n";
                std::cout << "password: " << password << "\n";
				
				if (!Initialize()) {
					printf("Failed to initialise Archipelago\n");
				}
			}
		}
		else if (line.find("/connect") == 0) {
			printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
		}
        else if (line.find("/disconnect") == 0) {
            if (ap) {
                delete ap;
                ap = nullptr;

                status = "Not connected\n";
                resetAP();
                system("cls");
                printf("You are now disconnected\n");
                printf("--------------------------------------------------------------------------------------------------\n");
                printf("Commands:\n");
                printf(" - '/help' - shows available commands\n");
                printf(" - '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' - to connect to the room\n");
                printf(" - '/disconnect' - to exit the room\n");
                printf("--------------------------------------------------------------------------------------------------\n");
            }
        }
    }
}

bool Initialize() {
    // Generate a uuid
    std::string uuid = ap_get_uuid(UUID_FILE);

    if (ap != nullptr) {
        ap->reset();
        delete ap;
        ap = nullptr;
    }

    ap = new APClient(uuid, "Perfect Dark", URI);

    ap->set_receive_own_locations(true);

    // load DataPackage cache
    FILE* f = fopen(DATAPACKAGE_CACHE, "rb");
    if (f) {
        char* buf = nullptr;
        size_t len = (size_t)0;
        if ((0 == fseek(f, 0, SEEK_END)) 
                && ((len = ftell(f)) > 0) 
                && ((buf = (char*)malloc(len+1))) 
                && (0 == fseek(f, 0, SEEK_SET)) 
                && (len == fread(buf, 1, len, f))) {
            buf[len] = 0;
        }
        free(buf);
        fclose(f);
    }

    ap_sync_queued = false;

    bool error = false;
    bool connected = false;
    bool roomInfo = false;
    bool roomUpdate = false;

    printf("Attempting to connect to room...\n");

    // Set event callbacks
    // Called when the socket gets connected
    ap->set_socket_connected_handler([&connected]() {
        printf("socket connected\n");
        connected = true;
        status = "Connected\n";
    });

    // Called when connect or a ping failed - no action required, reconnect is automatic
    ap->set_socket_error_handler([&error](const std::string& msg) {
        printf("socket error\n");
        error = true;
        status = "Not connected\n";
    });
	
    // Called when the socket gets disconnected - no action required, reconnect is automatic
    ap->set_socket_disconnected_handler([]() {
        printf("socket disconnected\n");
        status = "Disconnected\n";
    });

    // Called as reply to ConnectSlot when successful. argument is slot data.
	ap->set_slot_connected_handler([](const json& data) {
        ap->StatusUpdate(APClient::ClientStatus::PLAYING);
        printf("Connected and ready to go as %s\n", ap->get_player_alias(ap->get_player_number()).c_str());

        if (data.contains("options")) {
            if (data.at("options").contains("goal")) {
                completionGoal = data.at("options").at("goal");
            }

            if (data.at("options").contains("required_mission_stars")) {
                missionStars = 0;
                requiredMissionStars = data.at("options").at("required_mission_stars");
                cachedMissionStars = -1;
            }

            if (data.at("options").contains("weapon_progression")) {
                progressiveWeapon = 0;
                weaponProgressionType = data.at("options").at("weapon_progression");
            }

            if (data.at("options").contains("prog_weapon_in_challenges")) {
                allowProgWeaponInChallenges = data.at("options").at("prog_weapon_in_challenges");
            }

            if (data.at("options").contains("challenges")) {
                hasChallenges = data.at("options").at("challenges");
            }

            if (data.at("options").contains("weapon_training")) {
                hasWeaponTraining = data.at("options").at("weapon_training");
            }

            if (data.at("options").contains("device_training")) {
                hasDeviceTraining = data.at("options").at("device_training");
            }

            if (data.at("options").contains("holotraining")) {
                hasHolotraining = data.at("options").at("holotraining");
            }

            if (data.at("options").contains("deathlink")) {
                deathLink = data.at("options").at("deathlink");
            }
		}

        if (deathLink == 1) {
            ap->ConnectUpdate(false, 0b111, true, {"AP", "DeathLink"});
        }
	});
	
    // Called as reply to ConnectSlot failed. argument is reason.
    ap->set_slot_refused_handler([](const std::list<std::string>& errors){
        printf("AP: Connection refused:");
        for (const auto& error: errors) {
            printf(" %s", error.c_str());
        }
        printf("\n");
        status = "Not connected\n";
	});
    
    ap->set_slot_disconnected_handler([]() {
		printf("Slot disconnected\n");
        status = "Disconnected\n";
	});

    // Called when the server sent room info. send ConnectSlot from this callback
    ap->set_room_info_handler([&roomInfo]() {
        // printf("Server sent room info\n");
        std::list<std::string> tags;
        tags.push_back("AP");
        if (deathLink == 1) {
            tags.push_back("DeathLink");
        }
        ap->ConnectSlot(slotName, password, 0b111, tags, VERSION_TUPLE);
        roomInfo = true;
    });

    // Sent when there is a need to update information about the present game session
    ap->set_room_update_handler([&roomUpdate]() {
        roomUpdate = true;
    });

    // Called when receiving items - previously received after connect and new over time
	ap->set_items_received_handler([](const std::list<APClient::NetworkItem>& items) {
        int myPlayerNumber = ap->get_player_number();

        // Checks if data package seems to be valid for the server/room.
        if (!ap->is_data_package_valid()) {
            if (!ap_sync_queued) {
                ap->Sync();
            }
            ap_sync_queued = true;
            return;
        }

        for (const auto& item : items) {
            QueueItem(item);
        }
    });

    // Called as reply to LocationScouts
    ap->set_location_info_handler([](const std::list<APClient::NetworkItem>& items) {
        int playerNum = ap->get_player_number();
        for (auto item: items) {
            if (item.player != playerNum) {
                std::string itemname = ap->get_item_name(item.item, ap->get_player_game(item.player));
                std::string recipient = ap->get_player_alias(item.player);
                std::string location = ap->get_location_name(item.location, "");

                printSentItemMessage(itemname.c_str(), recipient.c_str(), location.c_str());
            }
        }
    });

    // Called when data package (texts) were updated from the server
    ap->set_data_package_changed_handler([](const json& data) {
        FILE* f = fopen(DATAPACKAGE_CACHE, "wb");
        if (f) {
            std::string s = data.dump();
            fwrite(s.c_str(), 1, s.length(), f);
            fclose(f);
        }
    });

    // legacy chat message
	ap->set_print_handler([](const std::string& msg) {
        printf("%s\n", msg.c_str());
    });

    // colorful chat and server messages. pass arg.data to render_json for text output
	ap->set_print_json_handler([](const std::list<APClient::TextNode>& msg) {
        printf("%s\n", ap->render_json(msg, APClient::RenderFormat::ANSI).c_str());
    });

    // broadcasted when a client sends a Bounce
	ap->set_bounced_handler([](const json& cmd) {
        if (deathLink == 1) {
            auto tagsIt = cmd.find("tags");
			auto dataIt = cmd.find("data");

            if (tagsIt != cmd.end() && tagsIt->is_array()
				&& std::find(tagsIt->begin(), tagsIt->end(), "DeathLink") != tagsIt->end())
			{
				if (dataIt != cmd.end() && dataIt->is_object()) {
					json data = *dataIt;
					if (data["source"].get<std::string>() != slotName) {
						std::string source = data["source"].is_string() ? data["source"].get<std::string>().c_str() : "???";
						std::string cause = data["cause"].is_string() ? data["cause"].get<std::string>().c_str() : "???";
						std::cout << "Killed by " << source << " - " << cause << "\n";
                        
                        printf("Received Death Link\n");
                        pendingDeathLink = true;
					}
				}
				else {
					printf("Bad deathlink packet!\n");
				}
			}
        }
    });

    // Called when a local location was remotely checked or was already checked when connecting
    ap->set_location_checked_handler([](const std::list<int64_t>& locations) {
        
    });

    // called as reply to Get
    ap->set_retrieved_handler([](const json& cmd) {
        
    });

    // called as reply to Set and when value for SetNotify changed
    ap->set_set_reply_handler([](const json& cmd) {
        
    });

    if (!connected) {
        fprintf(stderr, "FAIL: Could not connect socket\n");
        return false;
    }
    if (!roomInfo) {
        fprintf(stderr, "FAIL: Did not receive room info\n");
        return false;
    }
    if (!roomUpdate) {
        fprintf(stderr, "FAIL: Did not receive room update\n");
        return false;
    }
    if (error) {
        fprintf(stderr, "FAIL: Error\n");
        return false;
    }

    return true;
}

void Message(std::string message) {
	if (ap && ap->get_state() == APClient::State::SLOT_CONNECTED) {
		ap->Say(message);
	}
}

int IsConnected() {
	return ap && ap->get_state() == APClient::State::SLOT_CONNECTED;
}

void PollServer() {
    if (ap) {
        ap->poll();
    }
}

void ReachedGoal() {
	if (ap) {
        ap->StatusUpdate(APClient::ClientStatus::GOAL);
    }
}

void InternalCollectAPItem(uint64_t location)
{
    if (!ap) {
        return;
    }
    
    std::list<int64_t> check;
    check.push_back(location);
    ap->LocationScouts(check);
    ap->LocationChecks(check);
}

std::string itemNames[] = {
    "NONE",
    "UNARMED",
    "Falcon 2",
    "Falcon 2 (Silencer)",
    "Falcon 2 (Scope)",
    "MagSec 4",
    "Mauler",
    "Phoenix",
    "DY357 Magnum",
    "DY357-LX",
    "CMP150",
    "Cyclone",
    "Callisto NTG",
    "RC-P120",
    "Laptop Gun",
    "Dragon",
    "K7 Avenger",
    "AR34",
    "SuperDragon",
    "Shotgun",
    "Reaper",
    "Sniper Rifle",
    "FarSight XR-20",
    "Devastator",
    "Rocket Launcher",
    "Slayer",
    "Combat Knife",
    "Crossbow",
    "Tranquilizer",
    "Laser",
    "Grenade",
    "N-Bomb",
    "Timed Mine",
    "Proximity Mine",
    "Remote Mine",
    "Combat Boost",
	"PP9i",
	"CC13",
	"KL01313",
	"KF7 Special",
	"ZZT (9mm)",
	"DMC",
	"AR53",
	"RC-P45",
    "Psychosis Gun",
	"Night Vision",
	"CamSpy",
	"X-Ray Scanner",
	"IR Scanner",
	"Cloaking Device",
	"Horizon Scanner",
    "TESTER",
    "ROCKETLAUNCHER_34",
	"ECM Mine",
	"Data Uplink",
	"R-Tracker",
	"President Scanner",
	"Door Decoder",
	"Alien Medpack",
	"Explosives",
	"Skedar Bomb",
	"Comms Rider",
	"Tracer Bug",
	"Target Amplifier",
	"Lab Clothes",
	"Stewardess Disguise",
	"Flight Plans",
	"Research Tape",
	"Backup Disk",
	"G5 Building Level 1 Key Card",
	"G5 Building Level 2 Key Card",
	"Medlab 2 Key Card",
	"Op Room Key Card",
	"Air Force One Key Cards",
	"Cellar Key Card",
	"Area 51 Lift Key Card",
	"Cassandra's Office Key Card",
	"Suitcase",
	"WEAPON_BRIEFCASE",
	"Shield Tech Item",
	"De Vries' Necklace",
    "HAMMER",
    "SCREWDRIVER",
    "ROCKET",
    "HOMINGROCKET",
    "GRENADEROUND",
    "BOLT",
    "Briefcase",
	"SKROCKET",
    "CHOPPERGUN",
    "WATCHLASER",
    "Shield",
	"DISABLED",
    "SUICIDEPILL",
    "Defection - Agent",
    "Investigation - Agent",
    "Extraction - Agent",
    "Carrington Villa - Agent",
    "Chicago - Agent",
    "G5 Building - Agent",
    "Infiltration - Agent",
    "Rescue - Agent",
    "Escape - Agent",
    "Air Base - Agent",
    "Air Force One - Agent",
    "Crash Site - Agent",
    "Pelagic II - Agent",
    "Deep Sea - Agent",
    "Carrington Institute - Agent",
    "Attack Ship - Agent",
    "Skedar Ruins - Agent",
    "Mr. Blonde's Revenge - Agent",
    "Maian SOS - Agent",
    "WAR! - Agent",
    "The Duel - Agent",
    "Defection - Special Agent",
    "Investigation - Special Agent",
    "Extraction - Special Agent",
    "Carrington Villa - Special Agent",
    "Chicago - Special Agent",
    "G5 Building - Special Agent",
    "Infiltration - Special Agent",
    "Rescue - Special Agent",
    "Escape - Special Agent",
    "Air Base - Special Agent",
    "Air Force One - Special Agent",
    "Crash Site - Special Agent",
    "Pelagic II - Special Agent",
    "Deep Sea - Special Agent",
    "Carrington Institute - Special Agent",
    "Attack Ship - Special Agent",
    "Skedar Ruins - Special Agent",
    "Mr. Blonde's Revenge - Special Agent",
    "Maian SOS - Special Agent",
    "WAR! - Special Agent",
    "The Duel - Special Agent",
    "Defection - Perfect Agent",
    "Investigation - Perfect Agent",
    "Extraction - Perfect Agent",
    "Carrington Villa - Perfect Agent",
    "Chicago - Perfect Agent",
    "G5 Building - Perfect Agent",
    "Infiltration - Perfect Agent",
    "Rescue - Perfect Agent",
    "Escape - Perfect Agent",
    "Air Base - Perfect Agent",
    "Air Force One - Perfect Agent",
    "Crash Site - Perfect Agent",
    "Pelagic II - Perfect Agent",
    "Deep Sea - Perfect Agent",
    "Carrington Institute - Perfect Agent",
    "Attack Ship - Perfect Agent",
    "Skedar Ruins - Perfect Agent",
    "Mr. Blonde's Revenge - Perfect Agent",
    "Maian SOS - Perfect Agent",
    "WAR! - Perfect Agent",
    "The Duel - Perfect Agent",
	"Challenge 1",
	"Challenge 2",
	"Challenge 3",
	"Challenge 4",
	"Challenge 5",
	"Challenge 6",
	"Challenge 7",
	"Challenge 8",
	"Challenge 9",
	"Challenge 10",
	"Challenge 11",
	"Challenge 12",
	"Challenge 13",
	"Challenge 14",
	"Challenge 15",
	"Challenge 16",
	"Challenge 17",
	"Challenge 18",
	"Challenge 19",
	"Challenge 20",
	"Challenge 21",
	"Challenge 22",
	"Challenge 23",
	"Challenge 24",
	"Challenge 25",
	"Challenge 26",
	"Challenge 27",
	"Challenge 28",
	"Challenge 29",
	"Challenge 30",
    "Progressive Weapon",
    "Cheese",
    "Trap",
    "Mission Star",
    "Victory"
};

void QueueItem(APClient::NetworkItem item) {
    std::string itemname = ap->get_item_name(item.item, ap->get_player_game(item.player));
    if (ap->get_player_game(item.player) != "Perfect Dark") {
        itemname = itemNames[item.item - 1];
    }

	std::string sender = ap->get_player_alias(item.player);
	std::string location = ap->get_location_name(item.location, ap->get_player_game(item.player));

    handleItem(item.item, itemname.c_str(), sender.c_str(), location.c_str());
}

char SendDeathLink()
{
    if (!ap || deathLink == 0) {
        return 0;
    }

    double deathTime = ap->get_server_time();

    json data{
        {"time", deathTime},
        {"cause", "Agent Status: Deceased"},
        {"source", ap->get_slot()}
    };

    ap->Bounce(data, {}, {}, {"DeathLink"});
    return 1;
}

const char *GetServerAddress()
{
    return URI.c_str();
}

const char *GetSlotName()
{
    return slotName.c_str();
}

const char *GetPassword()
{
    return password.c_str();
}

const char *GetStatus()
{
    return status.c_str();
}