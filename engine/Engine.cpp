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
}
dll::PROTON::PROTON(FPOINT start_point, float width, float height) :start{ start_point }, _width { width }, _height{ height }
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;
}
dll::PROTON::PROTON(float sx, float sy, float width, float height) :start{ sx, sy }, _width{ width }, _height{ height }
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2;
	center.y = start.y + _height / 2;
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
}
void dll::PROTON::NewWidth(float new_width)
{
	_width = new_width;

	end.x = start.x + _width;

	center.x = start.x + _width / 2;
}
void dll::PROTON::NewHeight(float new_height)
{
	_height = new_height;

	end.y = start.y + _height;

	center.y = start.y + _height / 2;
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