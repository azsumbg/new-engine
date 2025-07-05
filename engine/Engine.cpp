#include "pch.h"
#include "Engine.h"

// RANDIt ******************************

dll::RANDIt::RANDIt()
{
	sq = new std::seed_seq{ rd(),rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	twister = new std::mt19937(*sq);
}
dll::RANDIt::~RANDIt()
{
	if (sq)delete(sq);
	if (twister)delete(twister);
}

int dll::RANDIt::operator()(int min, int max)
{
	std::uniform_int_distribution distrib(min, max);

	return distrib(*twister);
}

////////////////////////////////////////

// PROTON *****************************

dll::PROTON::PROTON()
{
	_width = 1.0f;
	_height = 1.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;

	x_radius = _width / 2;
	y_radius = _height / 2;
}
dll::PROTON::PROTON(FPOINT start_point, float width, float height) :start{ start_point }, _width { width }, _height{ height }
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;

	x_radius = _width / 2;
	y_radius = _height / 2;
}
dll::PROTON::PROTON(float sx, float sy, float width, float height) :start{ sx, sy }, _width{ width }, _height{ height }
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;

	x_radius = _width / 2;
	y_radius = _height / 2;
}

void dll::PROTON::SetEdges()
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;
}
void dll::PROTON::NewDims(float new_width, float new_height)
{
	_width = new_width;
	_height = new_height;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;

	x_radius = _width / 2;
	y_radius = _height / 2;
}
void dll::PROTON::NewWidth(float new_width)
{
	_width = new_width;

	end.x = start.x + _width;

	center.x = start.x + _width / 2;

	x_radius = _width / 2;
}
void dll::PROTON::NewHeight(float new_height)
{
	_height = new_height;

	end.y = start.y + _height;

	center.y = start.y + _height / 2;

	y_radius = _height / 2;
}

float dll::PROTON::GetWidth() const
{
	return _width;
}
float dll::PROTON::GetHeight() const
{
	return _height;
}

///////////////////////////////////////

// FIELD *****************************

dll::FIELD::FIELD(assets _type, float _sx, float _sy) :PROTON{ _sx, _sy, 1.0f, 1.0f }
{
	type = _type;

	switch (type)
	{
	case assets::platform1:
		NewDims(120.0f, 62.0f);
		break;

	case assets::platform2:
		NewDims(150.0f, 94.0f);
		break;

	case assets::platform3:
		NewDims(110.0f, 90.0f);
		break;

	case assets::potion:
		NewDims(20.0f, 20.0f);
		break;

	case assets::gold:
		NewDims(32.0f, 32.0f);
		break;

	case assets::tree:
		NewDims(55.0f, 60.0f);
		break;

	case assets::field:
		NewDims(800.0f, 750.0f);
		break;
	}
}
dll::FIELD::FIELD() :PROTON(0, 0, 0, 0)
{
	dir = dirs::stop;
	type = assets::no_type;
}
bool dll::FIELD::Move(float gear, dirs to_where)
{
	float my_speed = speed + gear / 5.0f;

	dir = to_where;

	if (dir == dirs::left)
	{
		start.x -= my_speed;
		SetEdges();
		if (end.x <= -scr_width)return false;
		return true;
	}
	if (dir == dirs::right)
	{
		start.x += my_speed;
		SetEdges();
		if (start.x >= 2 * scr_width)return false;
		return true;
	}

	return true;
}
void dll::FIELD::Release()
{
	delete this;
}

/////////////////////////////////////

// CREATURE *************************

void dll::CREATURE::SetPathInfo(float to_x, float to_y)
{
	move_sx = start.x;
	move_sy = start.y;

	move_ex = to_x;
	move_ey = to_y;

	hor_line = false;
	vert_line = false;

	if (move_sx == move_ex || (move_ex > start.x && move_ex < end.x))
	{
		vert_line = true;
		return;
	}
	if (move_sy == move_ey || (move_ey > start.y && move_ey < end.y))
	{
		hor_line = true;
		return;
	}

	slope = (move_ey - move_sy) / (move_ex - move_sx);
	intercept = start.y - slope * start.x;
}

dll::CREATURE::CREATURE(types _type, float _sx, float _sy, float _targ_x, float _targ_y) :PROTON{ _sx,_sy, 1.0f,1.0f }
{
	type = _type;

	switch (type)
	{
	case types::hero:
		NewDims(45.0f, 51.0f);
		max_frames = 4;
		frame_delay = 16;
		lifes = 100;
		speed = 0.2f;
		break;

	case types::evil1:
		NewDims(80.0f, 29.0f);
		lifes = 80;
		speed = 1.9f;
		sight_limit = 200;
		attack_delay = 320;
		max_frames = 11;
		frame_delay = 6;
		break;

	case types::evil2:
		NewDims(90.0f, 92.0f);
		lifes = 150;
		speed = 1.5f;
		sight_limit = 350;
		attack_delay = 340;
		max_frames = 28;
		frame_delay = 2;
		break;

	case types::evil3:
		NewDims(85.0f, 85.0f);
		lifes = 120;
		speed = 1.6f;
		sight_limit = 380;
		attack_delay = 330;
		max_frames = 20;
		frame_delay = 3;
		break;

	case types::evil4:
		NewDims(90.0f, 102.0f);
		lifes = 200;
		speed = 1.4f;
		sight_limit = 150;
		attack_delay = 360;
		max_frames = 5;
		frame_delay = 13;
		break;

	case types::evil5:
		NewDims(100.0f, 88.0f);
		lifes = 150;
		speed = 1.6f;
		sight_limit = 120;
		attack_delay = 350;
		max_frames = 16;
		frame_delay = 3;
		break;

	case types::evil6:
		NewDims(85.0f, 70.0f);
		lifes = 90;
		speed = 1.8f;
		sight_limit = 180;
		attack_delay = 340;
		max_frames = 23;
		frame_delay = 2;
		break;

	case types::axe:
		NewDims(20.0f, 20.0f);
		SetPathInfo(_targ_x, _targ_y);
		break;

	case types::fire:
		NewDims(20.0f, 20.0f);
		SetPathInfo(_targ_x, _targ_y);
		max_frames = 16;
		frame_delay = 3;
		break;
	}

	max_attack_delay = attack_delay;
}

int dll::CREATURE::GetFrame() 
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		switch (type)
		{
		case types::hero:
			frame_delay = 16;
			break;

		case types::evil1:
			frame_delay = 6;
			break;

		case types::evil2:
			frame_delay = 2;
			break;

		case types::evil3:
			frame_delay = 3;
			break;

		case types::evil4:
			frame_delay = 13;
			break;

		case types::evil5:
			frame_delay = 3;
			break;

		case types::evil6:
			frame_delay = 2;
			break;

		case types::fire:
			frame_delay = 3;
			break;
		}

		++frame;
		if (frame >= max_frames)frame = 0;
	}
	return frame;
}
void dll::CREATURE::Release()
{
	delete this;
}

bool dll::CREATURE::Contact(FIELD& what, dirs& where)
{
	
	if (!(what.start.x >= end.x || what.end.x <= start.x || what.start.y >= end.y || what.end.y <= start.y))
	{
		where = dirs::stop;

		unsigned char where_flag{ 0b00000000 };

		unsigned char up_flag{ 0b00000001 };
		unsigned char down_flag{ 0b00000010 };
		unsigned char left_flag{ 0b00000100 };
		unsigned char right_flag{ 0b00001000 };

		unsigned char up_left_flag{ 0b00000101 };
		unsigned char up_right_flag{ 0b00001001 };
		unsigned char down_left_flag{ 0b00000110 };
		unsigned char down_right_flag{ 0b00001010 };

		if (start.x < what.start.x)where_flag |= right_flag;
		else if (start.x >= what.start.x)where_flag |= left_flag;

		if (start.y < what.start.y)where_flag |= down_flag;
		else if (start.y >= what.start.y)where_flag |= up_flag;

		if (where_flag == up_left_flag)where = dirs::up_left;
		else if (where_flag == up_right_flag)where = dirs::up_right;
		else if (where_flag == down_left_flag)where = dirs::down_left;
		else if (where_flag == down_right_flag)where = dirs::down_right;
		else if (where_flag == left_flag)where = dirs::left;
		else if (where_flag == right_flag)where = dirs::right;
		else if (where_flag == up_flag)where = dirs::up;
		else if (where_flag == down_flag)where = dirs::down;

		return true;
	}

	return false;
}

void dll::CREATURE::Move(float gear)
{
	float my_speed = speed + gear / 5;
	dll::RANDIt Randerer;

	if (type == types::hero)
	{
		if (!jump)
		{
			switch (dir)
			{
			case dirs::left:
				if (start.x - my_speed >= 0)
				{
					start.x -= my_speed;
					SetEdges();
					state = states::move;
				}
				break;

			case dirs::right:
				if (end.x + my_speed <= scr_width)
				{
					start.x += my_speed;
					SetEdges();
					state = states::move;
				}
				break;
			}
		}
		else
		{
			if (!jump_start)
			{
				jump_start = true;
				jump_up = true;
				if (dir == dirs::right || dir == dirs::stop)
				{
					dir = dirs::right;
					SetPathInfo(end.x + 40.0f, start.y - 100.0f);
				}
				else if (dir == dirs::left) SetPathInfo(start.x - 40.0f, start.y - 100.0f);
				state = states::move;
			}
			else
			{
				if (jump_up)
				{
					if (vert_line)start.y -= my_speed;
					else if (hor_line)
					{
						if (move_ex < move_sx)start.x -= my_speed;
						else start.x += my_speed;
					}
					else if (dir == dirs::left)
					{
						if (start.x - my_speed >= 0)
						{
							start.x -= my_speed;
							start.y = start.x * slope + intercept;
						}
						else start.y -= my_speed;
					}
					else if (dir == dirs::right)
					{
						if (end.x + my_speed <= scr_width)
						{
							start.x += my_speed;
							start.y = start.x * slope + intercept;
						}
						else start.y -= my_speed;
					}

					SetEdges();

					if (start.y <= move_ey)
					{
						jump_up = false;
						if (dir == dirs::right)SetPathInfo(end.x, start.y + 100.0f);
						else if (dir == dirs::left)SetPathInfo(start.x, start.y + 100.0f);
					}
					state = states::move;
				}
				else
				{
					if (vert_line)start.y += my_speed;					
					else if (hor_line)
					{
						if (move_ex < move_sx)start.x -= my_speed;
						else start.x += my_speed;
					}
					else if (dir == dirs::left)
					{
						if (start.x - my_speed >= 0)
						{
							start.x -= my_speed;
							start.y = start.x * slope + intercept;
						}
						else start.y += my_speed;
					}
					else if (dir == dirs::right)
					{
						if (end.x + my_speed <= scr_width)
						{
							start.x += my_speed;
							start.y = start.x * slope + intercept;
						}
						else start.y += my_speed;
					}

					SetEdges();

					if (start.y >= move_ey)
					{
						jump_start = false;
						jump_up = false;
						jump = false;
						state = states::move;
					}
				}
			}
		}
	}
	else
	{
		switch (dir)
		{
		case dirs::left:
			if (start.x - my_speed >= -scr_width)
			{
				start.x -= my_speed;
				if ((type == types::evil2 || type == types::evil3) &&
					Randerer(0, 10) == 6 && end.y + 20.0f <= ground - 150.0f)start.y += 20.0f;
				SetEdges();
			}
			else dir = dirs::right;
			break;

		case dirs::right:
			if (end.x + my_speed <= scr_width * 2)
			{
				start.x += my_speed;
				if ((type == types::evil2 || type == types::evil3) && 
					Randerer(0, 10) == 6 && end.y + 20.0f <= ground - 150.0f)start.y += 20.0f;
				SetEdges();
			}
			else dir = dirs::left;
			break;

		case dirs::up:
			if (start.y - my_speed >= sky)
			{
				start.y -= my_speed;
				SetEdges();
			}
			else dir = dirs::down;
			break;

		case dirs::down:
			if (start.y <= ground)
			{
				start.y += my_speed;
				SetEdges();
			}
			else
			{
				start.y = ground;
				SetEdges();
				if (center.x >= scr_width / 2)dir = dirs::left;
				else dir = dirs::right;
				state = states::move;
			}
			break;
		}
	}
}
bool dll::CREATURE::Move(float gear, float targ_x, float targ_y) // IF targ_x == 0 && targ_y == 0 no SetPathInfo() !!!!!
{
	float my_speed = gear / 5 + speed;

	if (type == types::hero)return false;

	if (targ_x != 0 && targ_y != 0)SetPathInfo(targ_x, targ_y);

	if (hor_line)
	{
		if (move_ex > move_sx)
		{
			if (start.x + my_speed <= scr_width * 2)
			{
				start.x += my_speed;
				SetEdges();
				return true;
			}
			else return false;
		}
		else if (move_ex < move_sx)
		{
			if (end.x - my_speed >= -scr_width)
			{
				start.x -= my_speed;
				SetEdges();
				return true;
			}
			else return false;
		}
		else return false;
	}
	if (vert_line)
	{
		if (move_ey > move_sy)
		{
			if (start.y <= ground)
			{
				start.y += my_speed;
				SetEdges();
				return true;
			}
			else return false;
		}
		else if (move_ey < move_sy)
		{
			if (end.y - my_speed >= sky)
			{
				start.y -= my_speed;
				SetEdges();
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (move_ex < move_sx)start.x -= my_speed;
	else if (move_ex > move_sx)start.x += my_speed;

	start.y = start.x * slope + intercept;
	SetEdges();

	if (start.x >= scr_width * 2 || end.x <= -scr_width || start.y >= ground || end.y <= sky)return false;

	return true;
}

states dll::CREATURE::Dispatcher(FPOINT hero_point, BAG<FIELD>& objects)
{
	if (type == types::hero)return states::stop;

	BAG<FPOINT> obj_centers(objects.size());

	if (!objects.empty())
		for (int i = 0; i < objects.size(); i++)obj_centers.push_back(objects[i].center);
	
	dll::Sort(obj_centers, center);

	switch (state)
	{
	case states::stop:
		if (type != types::evil2 && type != types::evil3) // FLYERS
		{
			state = states::fall;
			dir = dirs::down;

			dirs contact_dir = dirs::stop;

			if (start.y > ground)
			{
				start.y = ground;
				SetEdges();
				state = states::move;
				if (dir == dirs::down)dir = dirs::left;
			} // ON THE GROUND

			if (((hero_point.x > end.x && hero_point.x - end.x <= sight_limit)
				|| (hero_point.x < start.x && start.x - hero_point.x <= sight_limit) || (hero_point.x >= start.x
					&& hero_point.x <= end.x)) &&
				((hero_point.y > end.y && hero_point.y - end.y <= sight_limit)
					|| (hero_point.y < start.y && start.y - hero_point.y <= sight_limit) ||
					(hero_point.y >= start.y && hero_point.y <= end.y)))

			{
				if (hero_point.x < start.x)dir = dirs::left;
				else dir = dirs::right;
				state = states::move;
			}
			
			if (!objects.empty())
			{
				for (int i = 0; i < objects.size(); ++i)
				{
					if (Contact(objects[i], contact_dir))
					{
						
						if (objects[i].type == assets::platform1 || objects[i].type == assets::platform2 ||
							objects[i].type == assets::platform3)
						{
							if (contact_dir == dirs::down || contact_dir == dirs::down_left || contact_dir == dirs::down_right)
							{
								start.y = objects[i].start.y - _height;
								SetEdges();
								state = states::move;
								if (hero_point.x < start.x)dir = dirs::left;
								else dir = dirs::right;
								break;
							}
							else if (contact_dir == dirs::left || contact_dir == dirs::down_left || contact_dir == dirs::up_left)
								dir = dirs::right;
							else if (contact_dir == dirs::right || contact_dir == dirs::down_right || contact_dir == dirs::up_right)
								dir = dirs::left;
							state = states::move;
					

							break;
						}
					}
				}
			}

			if (hero_point.x >= start.x && hero_point.x <= end.x && hero_point.y >= start.y && hero_point.y <= end.y)
				state = states::melee;
		}
		else if (((hero_point.x > end.x && hero_point.x - end.x <= sight_limit)
			|| (hero_point.x < start.x && start.x - hero_point.x <= sight_limit) || (hero_point.x >= start.x
				&& hero_point.x <= end.x)) &&
			((hero_point.y > end.y && hero_point.y - end.y <= sight_limit)
				|| (hero_point.y < start.y && start.y - hero_point.y <= sight_limit) ||
				(hero_point.y >= start.y && hero_point.y <= end.y)))state = states::attack;
		break;

	case states::move:
		if (type != types::evil2 && type != types::evil3)
		{
			state = states::fall;
			dir = dirs::down;

			dirs contact_dir = dirs::stop;

			if (start.y > ground)
			{
				start.y = ground;
				SetEdges();
				state = states::move;
				if (hero_point.x < start.x)dir = dirs::left;
				else dir = dirs::right;
			} // ON THE GROUND
			
			if (((hero_point.x > end.x && hero_point.x - end.x <= sight_limit)
				|| (hero_point.x < start.x && start.x - hero_point.x <= sight_limit) || (hero_point.x >= start.x
					&& hero_point.x <= end.x)) &&
				((hero_point.y > end.y && hero_point.y - end.y <= sight_limit)
					|| (hero_point.y < start.y && start.y - hero_point.y <= sight_limit) ||
					(hero_point.y >= start.y && hero_point.y <= end.y)))
			{
				if (hero_point.x < start.x)dir = dirs::left;
				else dir = dirs::right;
			}
			
			if (!objects.empty())
			{

				for (int i = 0; i < objects.size(); ++i)
				{
					if (Contact(objects[i], contact_dir))
					{
						if (objects[i].type == assets::platform1 || objects[i].type == assets::platform2 ||
							objects[i].type == assets::platform3)
						{
							if (contact_dir == dirs::down || contact_dir == dirs::down_left || contact_dir == dirs::down_right)
							{
								start.y = objects[i].start.y - _height;
								SetEdges();
								state = states::move;
								if (hero_point.x < start.x)dir = dirs::left;
								else dir = dirs::right;
								break;
							}
							else if (contact_dir == dirs::left || contact_dir == dirs::down_left || contact_dir == dirs::up_left)
								dir = dirs::right;
							else if (contact_dir == dirs::right || contact_dir == dirs::down_right || contact_dir == dirs::up_right)
								dir = dirs::left;
							
							state = states::move;

							break;
						}

					}
				}
			}

			if (hero_point.x >= start.x && hero_point.x <= end.x && hero_point.y >= start.y && hero_point.y <= end.y)
				state = states::melee;
		}
		else
		{
			state = states::move;

			dirs contact_dir = dirs::stop;

			if (!objects.empty())
			{
				for (int i = 0; i < objects.size(); ++i)
				{
					if (Contact(objects[i], contact_dir))
					{
						if (objects[i].type == assets::platform1 || objects[i].type == assets::platform2 ||
							objects[i].type == assets::platform3)
						{
							if (contact_dir == dirs::down || contact_dir == dirs::down_left || contact_dir == dirs::down_right)
							{
								start.y = objects[i].start.y - _height;
								SetEdges();
								state = states::move;
								break;
							}
						}
					}
				}
			}

			if (((hero_point.x > end.x && hero_point.x - end.x <= sight_limit)
				|| (hero_point.x < start.x && start.x - hero_point.x <= sight_limit) || (hero_point.x >= start.x
					&& hero_point.x <= end.x)) &&
				((hero_point.y > end.y && hero_point.y - end.y <= sight_limit)
					|| (hero_point.y < start.y && start.y - hero_point.y <= sight_limit) ||
					(hero_point.y >= start.y && hero_point.y <= end.y)))state = states::attack;

			if (hero_point.x >= start.x && hero_point.x <= end.x && hero_point.y >= start.y && hero_point.y <= end.y)
				state = states::melee;
		}
		break;
		
	case states::attack:
		--attack_delay;
		if (attack_delay <= 0)
		{
			attack_delay = max_attack_delay;
			state = states::attack_finished;
		}
		break;

	case states::melee:
		--attack_delay;
		if (attack_delay <= 0)
		{
			attack_delay = max_attack_delay;
			state = states::attack_finished;
		}
		break;

	case states::fall:
		if (start.y >= ground)
		{
			start.y = ground;
			SetEdges();
			state = states::move;
			if (start.x >= scr_width / 2)dir = dirs::left;
			else dir = dirs::right;
		} // ON THE GROUND
		else if (!objects.empty())
		{
			dirs contact_dir = dirs::stop;

			for (int i = 0; i < objects.size(); ++i)
			{
				if (Contact(objects[i], contact_dir))
				{
					if (objects[i].type == assets::platform1 || objects[i].type == assets::platform2 ||
						objects[i].type == assets::platform3)
					{
						if (contact_dir == dirs::down || contact_dir == dirs::down_left || contact_dir == dirs::down_right)
						{
							start.y = objects[i].start.y - _height;
							SetEdges();
							state = states::move;
							break;
						}
						else  if (contact_dir == dirs::left || contact_dir == dirs::down_left || contact_dir == dirs::up_left)
							dir = dirs::right;
						else if (contact_dir == dirs::right || contact_dir == dirs::down_right || contact_dir == dirs::up_right)
						dir = dirs::left;

						state = states::move;
						break;
					}
				}
			}
		}
		break;

	}

	return state;
}

// FUNCTIONS DEFINITION *******************************

float dll::Distance(FPOINT MyPoint, FPOINT TargetPoint)
{
	float a = static_cast<float>(pow(abs(TargetPoint.x - MyPoint.x), 2));
	float b = static_cast<float>(pow(abs(TargetPoint.y - MyPoint.y), 2));

	return static_cast<float>(sqrt(a + b));
}
bool dll::Sort(dll::BAG<FPOINT>& objects, FPOINT target)
{
	if (objects.size() < 2)return false;

	if (!objects.is_valid())return false;
	else
	{
		bool is_ok = false;

		while (!is_ok)
		{
			is_ok = true;

			for (size_t i = 0; i < objects.size() - 1; ++i)
			{
				if (Distance(objects[i], target) > Distance(objects[i + 1], target))
				{
					FPOINT temp{ objects[i] };
					objects[i] = objects[i + 1];
					objects[i + 1] = temp;
					is_ok = false;
					break;
				}
			}
		}

		return true;
	}

	return false;
}

ENGINE_API dll::FIELD*  dll::FieldFactory(assets mytype, float sx, float sy)
{
	FIELD* ret{ nullptr };

	ret = new FIELD(mytype, sx, sy);

	return ret;
}
ENGINE_API dll::CREATURE* dll::CreatureFactory(types mytype, float sx, float sy, float targ_x, float targ_y)
{
	CREATURE* ret{ nullptr };
	ret = new CREATURE(mytype, sx, sy, targ_x, targ_y);

	return ret;
}