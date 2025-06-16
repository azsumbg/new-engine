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
	
bool dll::FIELD::Move(float gear, dirs to_where)
{
	float my_speed = speed + gear / 5.0f;

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

	if (move_sx == move_ex || (move_ex > start.x && move_ex < end.x) || (move_ex < start.x && move_ex >= start.x - _width)
		|| (move_ex >= end.x && move_ex <= end.x + _width))
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
		speed = 1.0f;
		break;

	case types::evil1:
		NewDims(80.0f, 29.0f);
		lifes = 80;
		speed = 0.9f;
		sight_limit = 200;
		attack_delay = 50;
		max_frames = 11;
		frame_delay = 6;
		break;

	case types::evil2:
		NewDims(90.0f, 92.0f);
		lifes = 150;
		speed = 0.5f;
		sight_limit = 250;
		attack_delay = 80;
		max_frames = 28;
		frame_delay = 2;
		break;

	case types::evil3:
		NewDims(85.0f, 85.0f);
		lifes = 120;
		speed = 0.6f;
		sight_limit = 220;
		attack_delay = 70;
		max_frames = 20;
		frame_delay = 3;
		break;

	case types::evil4:
		NewDims(90.0f, 102.0f);
		lifes = 200;
		speed = 0.4f;
		sight_limit = 150;
		attack_delay = 100;
		max_frames = 5;
		frame_delay = 13;
		break;

	case types::evil5:
		NewDims(100.0f, 88.0f);
		lifes = 150;
		speed = 0.6f;
		sight_limit = 120;
		attack_delay = 90;
		max_frames = 16;
		frame_delay = 3;
		break;

	case types::evil6:
		NewDims(85.0f, 70.0f);
		lifes = 90;
		speed = 0.8f;
		sight_limit = 180;
		attack_delay = 40;
		max_frames = 23;
		frame_delay = 2;
		break;

	case types::axe:
		NewDims(20.0f, 20.0f);
		break;

	case types::fire:
		NewDims(20.0f, 20.0f);
		max_frames = 16;
		frame_delay = 3;
		break;
	}
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
	if (abs(what.center.x - center.x) <= what.x_radius + x_radius
		&& abs(what.center.y - center.y) <= what.y_radius + y_radius)
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


		if (start.x <= what.start.x)where_flag |= left_flag;
		else if (start.x >= what.end.x)where_flag |= right_flag;

		if (start.y <= what.start.y)where_flag |= up_flag;
		else if (start.y >= what.end.y)where_flag |= down_flag;

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

bool Move();
bool Move(float targ_x, float targ_y);

states Dispatcher(BAG<FPOINT>& creatures, BAG<FIELD>& objects);

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

dll::FIELD* ENGINE_API dll::FieldFactory(assets mytype, float sx, float sy)
{
	FIELD* ret{ nullptr };

	ret = new FIELD(mytype, sx, sy);

	return ret;
}
dll::CREATURE* ENGINE_API dll::CreatureFactory(assets mytype, float sx, float sy, float targ_x, float targ_y)
{



}