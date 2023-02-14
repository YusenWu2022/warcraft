//修改优化后的版本3.0  edited by Sarin.W ,2021/14/5
#include <vector> 
#include <iostream>
#include <string>
#include <cstdio>
using namespace std;
string Time;
//保持了时间和城市数组的全局变量（当然可以看成CWorld的对象变量，但并没有本质的区别）
class CFighter;
class CHeadquarter;//声明
class cities//城市类定义
{
public:
	int No;
	CFighter *Blue = NULL;  //城市中的红武士与蓝武士指针
	CFighter *Red = NULL;
	int life = 0;//城里的生命元
	bool red_occupied = false;//是否被插旗
	bool blue_occupied = false;
	void spring()//每小时生成生命元
	{
		life += 10;
	}
	string flag = "";//当前旗子，默认为空旗子
};
cities *city;
class CWeapon//武器类
{
public:
	int No;//编号
	int power;//（属于sword的）攻击力
	int R;//（属于arrow的）攻击力
	int used_time;//（属于arrow的）使用次数
	CWeapon(int _No, int _power, int _R, int _used_time = 0) : No(_No), power(_power), R(_R), used_time(_used_time){};
};
class CFighter//武士类
{
public:
	int K;//lion的忠诚度改变
	int life;//生命
	int force;//攻击力
	int city_No;//所在城市
	int loyalty;//忠诚度
	int iceman_walk = 0;//iceman已经走的步数
	string head_name;//所属headquarter
	int No_head;//编号

	bool reach_called = false;//到达对方大本营
	double spirit;//dragon的士气
	CWeapon *weapons[3] = {NULL};//持有武器的指针
	int life_before_fight;//记录战斗之前的生命，用于lion的生命传递
	CHeadquarter *belong;//所属head的指针
	string total_name;//便于输出，提前准备好总的称呼，格式如“red lion 1"
	string type_name;//种类名
	CFighter(int _life, int _force, int _cityNo, int _No_head, string _head_side) : life(_life), force(_force), city_No(_cityNo), No_head(_No_head), head_name(_head_side){																																				 };
	void report()//报告持有的武器
	{
		int tmp_count = 0;
		cout << Time << " " << total_name << " has ";
		if (weapons[2] != NULL)
		{
			cout << "arrow(" << 3 - weapons[2]->used_time << ")";
			tmp_count++;
		}

		if (weapons[1] != NULL)
		{
			if (tmp_count > 0)
				cout << ',';
			cout << "bomb";
			tmp_count++;
		}
		if (weapons[0] != NULL)
		{
			if (tmp_count > 0)
				cout << ",";
			cout << "sword(" << weapons[0]->power << ')';
			tmp_count++;
		}
		if (tmp_count == 0)
			cout << "no weapon";
		cout << endl;
	}
	virtual void march()//前进动作
	{
		if (head_name == "red")
		{
			city_No++;
			city[city_No].Red = &(*this);
			city[city_No - 1].Red = NULL;
		}
		else
		{
			city_No--;
			city[city_No].Blue = &(*this);
			city[city_No + 1].Blue = NULL;
		}
	};
	void march_call()//输出前进动作
	{
		cout << Time << " " << total_name << " marched to city " << city_No << " with " << life << " elements and force " << force << endl;
	}
	void reach_call()//输出到达对方大本营动作
	{
		cout << Time << " " << total_name << " reached ";
		if (head_name == "red")
			cout << "blue";
		else
			cout << "red";
		cout << " headquarter with " << life << " elements and force " << force << endl;
	}
	virtual void high_spirit(){};//打赢提高士气
	virtual void low_spirit(){};//没赢降低士气
	virtual void shot()//射箭动作，可能杀死
	{
		if (head_name == "red")
		{
			if (weapons[2] != NULL && city[city_No + 1].Blue != NULL)
			{
				city[city_No + 1].Blue->life -= weapons[2]->R;
				if (city[city_No + 1].Blue->life <= 0)
					cout << Time << " " << total_name << " shot and killed " << city[city_No + 1].Blue->total_name << endl;
				else
					cout << Time << " " << total_name << " shot" << endl;
				weapons[2]->used_time++;
				if (weapons[2]->used_time == 3)
					weapons[2] = NULL;
			}
		}
		else
		{
			if (weapons[2] != NULL && city[city_No - 1].Red != NULL)
			{
				city[city_No - 1].Red->life -= weapons[2]->R;
				if (city[city_No - 1].Red->life <= 0)
					cout << Time << " " << total_name << " shot and killed " << city[city_No - 1].Red->total_name << endl;
				else
					cout << Time << " " << total_name << " shot" << endl;
				weapons[2]->used_time++;
				if (weapons[2]->used_time == 3)
					weapons[2] = NULL;
			}
		}
	}
	void flag_check()//检查并改变应改变的所在城市旗子（包含计数）
	{

		if (head_name == "red")
		{
			if (city[city_No].red_occupied == true && city[city_No].flag != "red")
			{
				city[city_No].red_occupied = false;
				city[city_No].blue_occupied = false;
				city[city_No].flag = "red";
				cout << Time << " red flag raised in city " << city_No << endl;
			}
			else
			{
				city[city_No].red_occupied = true;
				city[city_No].blue_occupied = false;
			}
		}
		else
		{
			if (city[city_No].blue_occupied == true && city[city_No].flag != "blue")
			{
				city[city_No].blue_occupied = false;
				city[city_No].red_occupied = false;
				city[city_No].flag = "blue";
				cout << Time << " blue flag raised in city " << city_No << endl;
			}
			else
			{
				city[city_No].blue_occupied = true;
				city[city_No].red_occupied = false;
			}
		}
	}
	virtual void use_bomb()//判断是否使用bomb
	{
		//if (life <= 0)
		//return;
		if (weapons[1] != NULL)
		{

			if (head_name == "red")
			{
				int oppo_fight_attack;
				int oppo_attack;
				int my_attack;
				if (weapons[0] != NULL)
					my_attack = force + weapons[0]->power;
				else
					my_attack = force;
				if (city[city_No].Blue->weapons[0] != NULL)
					oppo_fight_attack = city[city_No].Blue->force / 2 + city[city_No].Blue->weapons[0]->power;
				else
					oppo_fight_attack = city[city_No].Blue->force / 2;
				if (city[city_No].Blue->weapons[0] != NULL)
					oppo_attack = city[city_No].Blue->force + city[city_No].Blue->weapons[0]->power;
				else
					oppo_attack = city[city_No].Blue->force;
				//bool use_or_not=false;
				if (((city[city_No].flag == "red" || (city[city_No].flag == "" && city_No % 2 == 1)) && (my_attack < city[city_No].Blue->life && oppo_fight_attack >= life) && city[city_No].Blue->type_name != "ninja") || ((city[city_No].flag == "blue" || (city[city_No].flag == "" && city_No % 2 == 0)) && (oppo_attack >= life)))
				{
					cout << Time << " " << total_name << " used a bomb and killed " << city[city_No].Blue->total_name << endl;
					//可能得再headquarter中间改一下
					city[city_No].Blue = NULL;
					city[city_No].Red = NULL;
				}
			}
			else
			{
				int oppo_fight_attack;
				int oppo_attack;
				int my_attack;
				if (weapons[0] != NULL)
					my_attack = force + weapons[0]->power;
				else
					my_attack = force;
				if (city[city_No].Red->weapons[0] != NULL)
					oppo_fight_attack = city[city_No].Red->force / 2 + city[city_No].Red->weapons[0]->power;
				else
					oppo_fight_attack = city[city_No].Red->force / 2;
				if (city[city_No].Red->weapons[0] != NULL)
					oppo_attack = city[city_No].Red->force + city[city_No].Red->weapons[0]->power;
				else
					oppo_attack = city[city_No].Red->force;
				if (((city[city_No].flag == "blue" || (city[city_No].flag == "" && city_No % 2 == 0)) && (my_attack < city[city_No].Red->life && city[city_No].Red->type_name != "ninja" && oppo_fight_attack >= life)) || ((city[city_No].flag == "red" || (city[city_No].flag == "" && city_No % 2 == 1)) && (oppo_attack >= life)))
				{
					cout << Time << " " << total_name << " used a bomb and killed " << city[city_No].Red->total_name << endl;
					//可能得再headquarter中间改一下
					city[city_No].Blue = NULL;
					city[city_No].Red = NULL;
				}
			}
		}
	}
	virtual void attack()//主动攻击动作
	{

		if (head_name == "red")
		{
			cout << Time << " " << total_name << " attacked " << city[city_No].Blue->total_name << " in city " << city_No << " with " << life << " elements and force " << force << endl;
			if (weapons[0] != NULL)
			{
				city[city_No].Blue->life -= force + weapons[0]->power;
				weapons[0]->power = (int)weapons[0]->power * 0.8;
				if (weapons[0]->power == 0)
					weapons[0] = NULL;
			}
			else
				city[city_No].Blue->life -= force;
			if (city[city_No].Blue->life <= 0)
			{
				cout << Time << " " << city[city_No].Blue->total_name << " was killed in city " << city_No << endl;
			}
		}

		else
		{
			cout << Time << " " << total_name << " attacked " << city[city_No].Red->total_name << " in city " << city_No << " with " << life << " elements and force " << force << endl;
			if (weapons[0] != NULL)
			{
				city[city_No].Red->life -= force + weapons[0]->power;
				weapons[0]->power = (int)weapons[0]->power * 0.8;
				if (weapons[0]->power == 0)
					weapons[0] = NULL;
			}
			else
				city[city_No].Red->life -= force;
			if (city[city_No].Red->life <= 0)
			{
				cout << Time << " " << city[city_No].Red->total_name << " was killed in city " << city_No << endl;
			}
		}
	}
	virtual void fight_back()//反击动作
	{
		if (head_name == "red")
		{
			cout << Time << " " << total_name << " fought back against " << city[city_No].Blue->total_name << " in city " << city_No << endl;
			if (weapons[0] != NULL)
			{
				city[city_No].Blue->life -= force / 2 + weapons[0]->power;
				weapons[0]->power = (int)weapons[0]->power * 0.8;
				if (weapons[0]->power == 0)
					weapons[0] = NULL;
			}
			else
				city[city_No].Blue->life -= force / 2;
			if (city[city_No].Blue->life <= 0)
			{
				cout << Time << " " << city[city_No].Blue->total_name << " was killed in city " << city_No << endl;
			}
			else
			{
				city[city_No].blue_occupied = false;
				city[city_No].red_occupied = false;
			}
		}

		else
		{
			cout << Time << " " << total_name << " fought back against " << city[city_No].Red->total_name << " in city " << city_No << endl;
			if (weapons[0] != NULL)
			{
				city[city_No].Red->life -= force / 2 + weapons[0]->power;
				weapons[0]->power = (int)weapons[0]->power * 0.8;
				if (weapons[0]->power == 0)
					weapons[0] = NULL;
			}
			else
				city[city_No].Red->life -= force / 2;
			if (city[city_No].Red->life <= 0)
			{
				cout << Time << " " << city[city_No].Red->total_name << " was killed in city " << city_No << endl;
			}
			else
			{
				city[city_No].blue_occupied = false;
				city[city_No].red_occupied = false;
			}
		}
	};
	void take_lion_life(CFighter *tmp)//死亡lion之前生命被对方获取
	{
		if (tmp->type_name == "lion" && tmp->life_before_fight > 0)
			life += tmp->life_before_fight;
	}
	virtual void check_run_away(){};//lion可能跑路
	virtual void low_loyalty(){};//lion没赢降低忠诚度
	virtual void rob(CFighter *aim){};//wolf胜利后抢夺武器
	virtual void cheer(){};//dragon主动没死的欢呼
};
class CDragon : public CFighter//龙类
{
public:
	CDragon(int _life, int _force, int _cityNo, float _spirit, int _No_head, string _head_name, CWeapon *_weapons) : CFighter(_life, _force, _cityNo, _No_head, _head_name)
	{
		spirit = _spirit;
		weapons[_weapons->No] = _weapons;
		if (weapons[0] != NULL && weapons[0]->power == 0)
			weapons[0] = NULL;
		type_name = "dragon";
		string tmp_No;
		//use "to_string" function to get a string looks like integer.
		total_name = head_name + " " + type_name + " " + to_string(No_head);
		cout << Time << " " << total_name << " born" << endl;
		cout << "Its morale is ";
		printf("%.2f\n", spirit);
	};
	void cheer()
	{
		if (spirit > 0.8)
			cout << Time << " " << total_name << " yelled in city " << city_No << endl;
	}
	void high_spirit()
	{
		spirit += 0.2;
	}
	void low_spirit()
	{
		spirit -= 0.2;
	}
};
class CLion : public CFighter//狮子类
{
public:
	CLion(int _K, int _life, int _force, int _cityNo, int _No_head, string _head_name, int _loyalty) : CFighter(_life, _force, _cityNo, _No_head, _head_name)
	{
		loyalty = _loyalty;
		K = _K;
		type_name = "lion";
		total_name = head_name + " " + type_name + " " + to_string(No_head);
		cout << Time << " " << total_name << " born" << endl;
		cout << "Its loyalty is " << loyalty << endl;
	};
	void check_run_away()
	{
		if (loyalty <= 0)
		{
			cout << Time << " " << total_name << " ran away" << endl;
			if (head_name == "red")
				city[city_No].Red = NULL;
			else
				city[city_No].Blue = NULL;
		}
		//暂不计入在headquarter中的记录改变
	}
	void low_loyalty()
	{
		loyalty -= K;
	}
};
class CIceman : public CFighter//冰人类
{
public:
	CIceman(int _life, int _force, int _cityNo, int _No_head, string _head_name, CWeapon *_weapons) : CFighter(_life, _force, _cityNo, _No_head, _head_name)
	{
		weapons[_weapons->No] = _weapons;
		type_name = "iceman";
		total_name = head_name + " " + type_name + " " + to_string(No_head);
		cout << Time << " " << total_name << " born" << endl;
		if (weapons[0] != NULL && weapons[0]->power == 0)
			weapons[0] = NULL;
	};
	virtual void march()//每两步改变生命增强战斗力，用了多态
	{
		iceman_walk++;

		if (head_name == "red")
		{
			city_No++;
			city[city_No].Red = &(*this);
			city[city_No - 1].Red = NULL;
		}
		else
		{
			city_No--;
			city[city_No].Blue = &(*this);
			city[city_No + 1].Blue = NULL;
		}
		if (iceman_walk == 2)
		{
			force += 20;
			if (life > 9)
				life -= 9;
			else
				life = 1;
			iceman_walk = 0;
		}
	}
};
class CNinja : public CFighter//忍者类
{
public:
	CNinja(int _life, int _force, int _cityNo, int _No_head, string _head_name, CWeapon *_weapons, CWeapon *for_ninja) : CFighter(_life, _force, _cityNo, _No_head, _head_name)
	{
		weapons[_weapons->No] = _weapons;
		weapons[for_ninja->No] = for_ninja;
		type_name = "ninja";
		total_name = head_name + " " + type_name + " " + to_string(No_head);
		cout << Time << " " << total_name << " born" << endl;
		if (weapons[0] != NULL && weapons[0]->power == 0)
			weapons[0] = NULL;
	};
	virtual void fight_back()//不反击
	{
		return;
	}
};
class CWolf : public CFighter//狼类
{
public:
	CWolf(int _life, int _force, int _cityNo, int _No_head, string _head_name) : CFighter(_life, _force, _cityNo, _No_head, _head_name)
	{
		type_name = "wolf";
		total_name = head_name + " " + type_name + " " + to_string(No_head);
		cout << Time << " " << total_name << " born" << endl;
	};
	void rob(CFighter *aim)//抢夺
	{

		bool have_weapon[3] = {false};
		for (int i = 0; i <= 2; i++)
			if (weapons[i] != NULL)
				have_weapon[weapons[i]->No] = true;
		for (int i = 0; i <= 2; i++)
			if (aim->weapons[i] != NULL && have_weapon[aim->weapons[i]->No] == false)
			{
				weapons[aim->weapons[i]->No] = aim->weapons[i];
				have_weapon[aim->weapons[i]->No] = true;
			}
	}
};
class CHeadquarter//大本营类
{
public:
	CFighter *fighter;
	string head_name;//名称
	int life_base;//总生命元
	int city_No;//所在城市
	int R;
	int K;
	int award_num;//需要奖励的武士数目
	bool occupied = false;//是否被占领
	int produce_need_life[6];//生产武士需要的生命元
	int produce_force[6];//生产武士的攻击力
	vector<int> award_No;//需要奖励以及获取战利品的武士列表
	string produce_order_name[6];//生产顺序
	int produce_now;//当前生产的武士编号
	int occupied_process = 0;//被占领次数
	CFighter *fighter_list[1000];
	int fighter_No;//拥有武士列表
	CHeadquarter(int _city_No, string _head_name, int _life_base, int _R, int _K, int *_origin_life, int *_origin_force) : city_No(_city_No), head_name(_head_name), life_base(_life_base), R(_R), K(_K)
	{
		fighter_No = 1;
		produce_now = 1;

		if (head_name == "red")
		{
			produce_need_life[1] = _origin_life[3];
			produce_need_life[2] = _origin_life[4];
			produce_need_life[3] = _origin_life[5];
			produce_need_life[4] = _origin_life[2];
			produce_need_life[5] = _origin_life[1];
			produce_force[1] = _origin_force[3];
			produce_force[2] = _origin_force[4];
			produce_force[3] = _origin_force[5];
			produce_force[4] = _origin_force[2];
			produce_force[5] = _origin_force[1];
			produce_order_name[1] = "iceman";
			produce_order_name[2] = "lion";
			produce_order_name[3] = "wolf";
			produce_order_name[4] = "ninja";
			produce_order_name[5] = "dragon";
		}
		else
		{
			produce_need_life[1] = _origin_life[4];
			produce_need_life[2] = _origin_life[1];
			produce_need_life[3] = _origin_life[2];
			produce_need_life[4] = _origin_life[3];
			produce_need_life[5] = _origin_life[5];
			produce_force[1] = _origin_force[4];
			produce_force[2] = _origin_force[1];
			produce_force[3] = _origin_force[2];
			produce_force[4] = _origin_force[3];
			produce_force[5] = _origin_force[5];
			produce_order_name[1] = "lion";
			produce_order_name[2] = "dragon";
			produce_order_name[3] = "ninja";
			produce_order_name[4] = "iceman";
			produce_order_name[5] = "wolf";
		}
	};
	void take(int city_No)
	{
		life_base += city[city_No].life;
		city[city_No].life = 0;
	}
	void award(CFighter *aim)//奖励
	{
		if (life_base >= 8)
		{
			aim->life += 8;
			life_base -= 8;
		}
	}
	bool check_occupied()//检查是否占领
	{
		return occupied_process >= 2;
	}
	CWeapon *make_weapon(int No, int power, int R)//同时生产武器
	{
		CWeapon *tmp = new CWeapon(No, power, R);
		return tmp;
	}
	void earn(int _city_No, CFighter *taker)//收集战利品
	{
		if (city[_city_No].life > 0)
		{
			life_base += city[_city_No].life;
			city[_city_No].life = 0;
		}
	}
	void produce()//生产武士
	{
		if (produce_need_life[produce_now] <= life_base)
		{

			life_base -= produce_need_life[produce_now];
			switch (produce_order_name[produce_now][0])
			{
			case 'd': //int _life, int _force, int _cityNo, float _spirit, int _No_head, string _head_name, CWeapon *_weapons
				fighter_list[++fighter_No] = new CDragon(produce_need_life[produce_now], produce_force[produce_now], city_No, double(life_base) / produce_need_life[produce_now], fighter_No, head_name, make_weapon(fighter_No % 3, produce_force[produce_now] / 5, R));
				break;
			case 'i':
				fighter_list[++fighter_No] = new CIceman(produce_need_life[produce_now], produce_force[produce_now], city_No, fighter_No, head_name, make_weapon(fighter_No % 3, produce_force[produce_now] / 5, R));
				break;
			case 'n':
				fighter_list[++fighter_No] = new CNinja(produce_need_life[produce_now], produce_force[produce_now], city_No, fighter_No, head_name, make_weapon(fighter_No % 3, produce_force[produce_now] / 5, R), make_weapon((fighter_No + 1) % 3, produce_force[produce_now] / 5, R));
				break;
			case 'l':
				fighter_list[++fighter_No] = new CLion(K, produce_need_life[produce_now], produce_force[produce_now], city_No, fighter_No, head_name, life_base);
				break;
			case 'w':
				fighter_list[++fighter_No] = new CWolf(produce_need_life[produce_now], produce_force[produce_now], city_No, fighter_No, head_name);
				break;
				///////////////////////////////
			}
			if (head_name == "red")
				city[city_No].Red = fighter_list[fighter_No];
			else
				city[city_No].Blue = fighter_list[fighter_No];
			produce_now++;
			if (produce_now == 6)
				produce_now = 1;
		}
	};
};
class CWorld
{
public:
	CHeadquarter *red, *blue;
	int time_trace;//当前分钟数0~60
	int city_num;//总城市数
	int t_limit;//总时间上限
	CWorld()//初始化世界
	{
		int M, N, R, K, T;
		int origin_life[6];
		int origin_force[6];
		cin >> M >> N >> R >> K >> T;//输入世界的演化参数，（有点像MINECRAFT中的“种子z",小差异会导致世界演化的千姿百态）
		city_num = N;
		t_limit = T;
		for (int i = 1; i <= 5; i++)
			cin >> origin_life[i];
		for (int i = 1; i <= 5; i++)
			cin >> origin_force[i];
		red = new CHeadquarter(0, "red", M, R, K, origin_life, origin_force);
		blue = new CHeadquarter(N + 1, "blue", M, R, K, origin_life, origin_force);

		time_trace = 0;
		city = new cities[city_num + 2];
	}
	void history() //history:书写历史(程序主体)
	{
		int hour_passed = 0;
		string time_hour;
		string time_minute;
		bool flag = false;
		time_hour = "000";
		time_minute = "00";//初始化
		while (hour_passed * 60 + time_trace <= t_limit)//时间循环
		{
			Time = time_hour + ":" + time_minute;
			switch (time_trace)
			{

			case 0://生产
				red->produce();
				blue->produce();
				break;
			case 5://狮子跑路
				for (int i = 0; i <= city_num+1; i++)
				{
					if (city[i].Red != NULL && city[i].Red->type_name == "lion")
						city[i].Red->check_run_away();
					if (city[i].Blue != NULL && city[i].Blue->type_name == "lion")
						city[i].Blue->check_run_away();
				}
				break;
			case 10://检查占领及移动动作   ***难点1，输出顺序要注意，否则可能导致覆盖，反向甚至错误
				if (city[city_num].Red != NULL)
					blue->occupied_process++;
				if (city[1].Blue != NULL)
					red->occupied_process++;
				for (int i = city_num + 1; i >= 0; i--)
				{
					if (city[i].Red != NULL) //
					{
						if (i != city_num + 1)
							city[i].Red->march();
					}
				}
				for (int i = 0; i <= city_num + 1; i++)
				{
					//later move may cover the move before!!

					if (city[i].Blue != NULL)
					{

						if (i != 0)
							city[i].Blue->march(); //attention!!!  here need some repair for the output should be reconsidered!!
					}
				}
				if (city[0].Blue != NULL && city[0].Blue->reach_called == false)
				{
					city[0].Blue->reach_call();
					city[0].Blue->reach_called = true;
				}
				if (red->check_occupied())//判断占领
				{
					cout << Time << " red headquarter was taken" << endl;
					flag = true;
				}
				for (int i = 1; i <= city_num; i++)
				{
					if (city[i].Red != NULL)
						city[i].Red->march_call();
					if (city[i].Blue != NULL)
						city[i].Blue->march_call();
				}

				if (city[city_num + 1].Red != NULL && city[city_num + 1].Red->reach_called == false)
				{
					city[city_num + 1].Red->reach_call();
					city[city_num + 1].Red->reach_called = true;
				}
				if (blue->check_occupied())
				{
					cout << Time << " blue headquarter was taken" << endl;
					flag = true;
				}

				break;
			case 20://产生生命元
				for (int i = 1; i <= city_num; i++)
					city[i].life += 10;
				break;
			case 30://只有一个人城市获取生命元
				for (int i = 0; i <= city_num + 1; i++)
				{
					if (city[i].Red != NULL && city[i].Blue == NULL)
					{
						if (city[i].life > 0)
							cout << Time << " " << city[i].Red->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
						red->earn(i, city[i].Red);
					}
					if (city[i].Blue != NULL && city[i].Red == NULL)
					{
						if (city[i].life > 0)
							cout << Time << " " << city[i].Blue->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
						blue->earn(i, city[i].Blue);
					}
				}
				break;
			case 35://射箭，保证下一个城市有敌军，  ***且不能朝大本营射箭
				for (int i = 0; i <= city_num + 1; i++)
				{
					if (city[i].Red != NULL && i != city_num && i != city_num + 1) //like here may consider use "inline" type fntion!(to reduce time to apply for the funtion)
						city[i].Red->shot();
					if (city[i].Blue != NULL && i != 1 && i != 0)
						city[i].Blue->shot();
				}
				break;
			case 38://判断使用炸弹  ***难点2  注意使用条件，“判断自己将在战斗中死亡”指考虑了先攻击方之下的情况（就是完全仿真模拟互相攻击的准确情况，是没有随机性的！！）
				for (int i = 0; i <= city_num; i++)
				{
					if (city[i].Red != NULL && city[i].Blue != NULL && city[i].Red->life > 0 && city[i].Blue->life > 0)
						city[i].Red->use_bomb();
					if (city[i].Red != NULL && city[i].Blue != NULL && city[i].Red->life > 0 && city[i].Blue->life > 0)
						city[i].Blue->use_bomb();
				}
				break;
			case 40://难点3  **战斗过程的判断以及一方胜利后需要做的事情都要按清晰的顺序依次不缺不漏地进行！
			//像升旗，抢武器，继承生命元，获取战利品，奖励生命元，输出攻击和反击结果等等。并且要保证所有情况都考虑在内！！这是最麻烦的部分
			{
				
				red->award_No.clear();
				blue->award_No.clear();
				red->award_num = 0;
				blue->award_num = 0;
				//大量的分类讨论：：：：
				for (int i = 0; i <= city_num + 1; i++)
				{
					if (city[i].Red != NULL && city[i].Blue == NULL && city[i].Red->life <= 0)
						city[i].Red = NULL;
					if (city[i].Blue != NULL && city[i].Red == NULL && city[i].Blue->life <= 0)
						city[i].Blue = NULL;
					if (city[i].Red != NULL && city[i].Blue != NULL)
					{
						city[i].Red->life_before_fight = city[i].Red->life;
						city[i].Blue->life_before_fight = city[i].Blue->life;
						if (city[i].Red->life <= 0 && city[i].Blue->life > 0)
						{
							if (city[i].flag == "blue" || (city[i].flag != "red" && i % 2 == 0))
								city[i].Blue->cheer();
							if (city[i].life > 0)
								cout << Time << " " << city[i].Blue->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;

							city[i].Blue->flag_check();
							blue->award_No.push_back(i);
							blue->award_num++;
							city[i].Blue->take_lion_life(city[i].Red);
							//city[i].blue_occupied = true;
							city[i].Blue->rob(city[i].Red);
							city[i].Red = NULL;
							city[i].Blue->high_spirit();
						}
						else if (city[i].Red->life <= 0 && city[i].Blue->life <= 0)
						{
							city[i].Red = NULL;
							city[i].Blue = NULL;
							
						}
						else if (city[i].Blue->life <= 0 && city[i].Red->life > 0)
						{
							if (city[i].flag == "red" || (city[i].flag != "blue" && i % 2 == 1))
								city[i].Red->cheer();
							if (city[i].life > 0)
								cout << Time << " " << city[i].Red->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
							city[i].Red->flag_check();
							red->award_No.push_back(i);
							red->award_num++;
							city[i].Red->rob(city[i].Blue);
							city[i].Red->take_lion_life(city[i].Blue);
							city[i].Blue = NULL;
							city[i].Red->high_spirit();
						}
						else if (city[i].Red->life > 0 && city[i].Blue->life > 0 && (i % 2 == 1 && city[i].flag != "blue") || city[i].flag == "red")
						{
							if (city[i].Blue->life > 0)
								city[i].Red->attack();
							if (city[i].Blue->life > 0)
							{
								city[i].Red->low_loyalty();
								city[i].Red->low_spirit();
								city[i].Blue->fight_back();
								if (city[i].Red->life <= 0)
								{

									if (city[i].life > 0)
										cout << Time << " " << city[i].Blue->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
									city[i].Blue->flag_check();
									blue->award_No.push_back(i);
									blue->award_num++;
									city[i].Blue->take_lion_life(city[i].Red);
									//city[i].blue_occupied = true;
									city[i].Blue->rob(city[i].Red);
									city[i].Red = NULL;
									city[i].Blue->high_spirit();
								}
								else
								{
									if (city[i].Red != NULL && city[i].Red->life > 0)
										city[i].Red->cheer();
									city[i].Blue->low_spirit();
									city[i].Blue->low_loyalty();
									city[i].red_occupied = false;
									city[i].blue_occupied = false;
								}
							}
							else
							{

								if (city[i].Red != NULL && city[i].Red->life > 0)
									city[i].Red->cheer();
								if (city[i].life > 0)
									cout << Time << " " << city[i].Red->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
								city[i].Red->flag_check();
								red->award_No.push_back(i);
								red->award_num++;
								city[i].Red->rob(city[i].Blue);
								city[i].Red->take_lion_life(city[i].Blue);
								city[i].Blue = NULL;
								city[i].Red->high_spirit();
							}
						}
						else if (city[i].Red->life > 0 && city[i].Blue->life > 0 && (i % 2 == 0 && city[i].flag != "red") || city[i].flag == "blue")
						{
							city[i].Blue->attack();
							if (city[i].Red->life > 0)
							{

								city[i].Blue->low_loyalty();
								city[i].Blue->low_spirit();
								city[i].Red->fight_back();
								if (city[i].Blue->life <= 0)
								{
									if (city[i].life > 0)
										cout << Time << " " << city[i].Red->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
									city[i].Red->flag_check();
									red->award_No.push_back(i);
									city[i].Red->take_lion_life(city[i].Blue);
									red->award_num++;
									city[i].Red->rob(city[i].Blue);
									city[i].Blue = NULL;
									city[i].Red->high_spirit();
								}
								else
								{
									if (city[i].Blue != NULL && city[i].Blue->life > 0)
										city[i].Blue->cheer();
									city[i].Red->low_spirit();
									city[i].Red->low_loyalty();
									city[i].red_occupied = false;
									city[i].blue_occupied = false;
								}
							}
							else
							{
								if (city[i].Blue != NULL && city[i].Blue->life > 0)
									city[i].Blue->cheer();
								if (city[i].life > 0)
									cout << Time << " " << city[i].Blue->total_name << " earned " << city[i].life << " elements for his headquarter" << endl;
								city[i].Blue->flag_check();
								blue->award_No.push_back(i);
								city[i].Blue->take_lion_life(city[i].Red);
								blue->award_num++;
								city[i].Blue->rob(city[i].Red);
								city[i].Red = NULL;
								city[i].Blue->high_spirit();
							}
						}
					}
				}
				int i = red->award_num - 1;
				while (red->award_num != 0 && i >= 0)
				{

					red->award(city[red->award_No[i]].Red);
					i--;
				}
				i = 0;
				while (i < red->award_num)
				{
					red->earn(red->award_No[i], city[red->award_No[i]].Red);
					i++;
				}

				i = 0;
				while (i < blue->award_num)
				{
					blue->award(city[blue->award_No[i]].Blue);
					i++;
				}
				i = 0;
				while (i < blue->award_num)
				{
					blue->earn(blue->award_No[i], city[blue->award_No[i]].Blue);
					i++;
				}
			}

			break;
			case 50://统计head的生命元
				cout << Time << " " << red->life_base << " elements in red headquarter" << endl;
				cout << Time << " " << blue->life_base << " elements in blue headquarter" << endl;
				break;
			case 55://按题給顺序逐个输出武士装备
				for (int i = 0; i <= city_num + 1; i++)
				{
					if (city[i].Red != NULL)
						city[i].Red->report();
				}
				for (int i = 0; i <= city_num + 1; i++)
				{
					if (city[i].Blue != NULL)
						city[i].Blue->report();
				}

				break;
			case 60://时间统一化，合成出string类型的时间便于输出
				hour_passed++;
				time_hour[0] = hour_passed / 100 + '0';
				time_hour[1] = (hour_passed / 10) % 10 + '0';
				time_hour[2] = hour_passed % 10 + '0';
				time_trace = -1;
				break;
			}
			if (flag == true)
				break;
			time_trace++;
			time_minute[0] = time_trace / 10 + '0';
			time_minute[1] = time_trace % 10 + '0';
		}
	}
};
int main()//主函数，初始化接口
{
	int T, N, t;
	cin >> T;
	int count = 0;
	while (T--)//遍历所有的case给出结果
	{
		count++;
		cout << "Case " << count << ":" << endl;
		CWorld new_world;

		new_world.history();
	}
	system("pause");
	return 0;
}
