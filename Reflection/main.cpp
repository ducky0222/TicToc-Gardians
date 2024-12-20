#include <fstream>
#include <iostream>
#include <random>

#include "TypeInfoMacro.h"
#include "PropertyMacro.h"

#include <nlohmann/json.hpp>
#include <glassify/glassify.h>

#include "JsonSerailizer.h"

using json = nlohmann::json;

struct Vector3
{
	GENERATE_CLASS_TYPE_INFO(Vector3)
		PROPERTY(x)
		PROPERTY(y)
		PROPERTY(z)

public:
	float x{}, y{}, z{};

	Vector3() {}
	Vector3(float a, float b, float c)
		: x(a), y(b), z(c) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vector3, x, y, z)
};

struct Quaternion
{
	GENERATE_CLASS_TYPE_INFO(Quaternion)
		PROPERTY(x)
		PROPERTY(y)
		PROPERTY(z)
		PROPERTY(w)

public:
	float x{}, y{}, z{}, w{};

	Quaternion() {}
	Quaternion(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Quaternion, x, y, z, w)
};

struct Transform
{
	GENERATE_CLASS_TYPE_INFO(Transform)
		PROPERTY(position)
		PROPERTY(rotation)
		PROPERTY(scale)

public:
	Vector3 position{};
	Quaternion rotation{};
	Vector3 scale{};

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Transform, position, rotation, scale)
};

struct TestParent
{
	GENERATE_CLASS_TYPE_INFO(TestParent)
	PROPERTY(parentString)
	PROPERTY(parentDouble)

public:
	std::string parentString;
	float parentDouble;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TestParent, parentString, parentDouble)
};

struct TestChild : TestParent
{
	GENERATE_CLASS_TYPE_INFO(TestChild)
	PROPERTY(childBool)
	PROPERTY(childString)

public:
	bool childBool;
	std::string childString;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TestChild, childBool, childString, parentString, parentDouble)
};

int main()
{
	Transform t1;
	int dd = 3;

	


	//auto& typeInfo = Transform::StaticTypeInfo();

	//auto p1 = typeInfo.GetProperty("position");
	//auto pp1 = p1->GetTypeInfo().GetProperty("x");
	//auto p2 = typeInfo.GetProperty("rotation");
	//auto p3 = typeInfo.GetProperty("scale");



	//auto gtype = glas::GetTypeInfo<Vector3>();
	//for (auto& mem : gtype.Members)
	//{
	//	std::cout << mem.Name << std::endl;
	//}

	//
	//Transform tt;

	//p1->Set(&tt, Vector3{ 20.f, 30.f, 40.f });
	//pp1->Set(&tt, 999.f);
	//p2->Set(&tt, Quaternion{ 20.f, 30.f, 40.f, 50.f });
	//p2->Set(&tt, Quaternion{ 20.f, 30.f, 40.f, 50.f });
	//p3->Set(&tt, Vector3{ 20.f, 30.f, 40.f });

	//json jsonObj = tt;

	//std::cout << jsonObj.dump(2) << std::endl;

	//auto x = typeInfo.GetProperty("position")->Get<Vector3>(&tt);

	//x.Get().y = 0.f;
	//// json을 파일로 저장
	//std::ofstream outputFile("../Data/Pool/Transform.json");
	//outputFile << jsonTransformPool.dump(2);  // 두 칸 들여쓰기로 보기 좋게 저장
	//outputFile.close();


	//// JSON 파일 읽기
	//std::ifstream inputFile("../Data/Pool/Transform.json");
	//if (!inputFile.is_open())
	//{
	//	std::cerr << "Failed to open JSON file." << std::endl;
	//	return 1;  // 파일 열기 실패 시 종료
	//}

	//json jsonTransformPool;
	//inputFile >> jsonTransformPool;


	std::map<size_t, TestChild> children;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> floatDistribution(0.0, 100.0);
	std::uniform_int_distribution<int> boolDistribution(0, 1);
	std::uniform_int_distribution<int> stringSizeDistribution(5, 15);

	for (size_t i = 0; i < 100; ++i) {


		TestChild obj;
		obj.parentString = "Parent" + std::to_string(i);
		obj.parentDouble = floatDistribution(gen);
		obj.childBool = boolDistribution(gen);
		obj.childString = "Child" + std::to_string(i);

		children.insert({i, std::move(obj)});
	}

	std::array<Transform, 32> array;
	auto c1 = SerializeContainer(array);
	auto serializeChildren = SerializeContainer(children);

	std::cout << serializeChildren << std::endl;
	auto deserializedVector = DeserializeContainerFromFile<std::list<Transform>>("../Data/Pool/Transform.json");

	for(auto& data : deserializedVector)
	{
		std::cout << "position : { " << data.position.x << data.position.y << data.position.z << " }\n";
		std::cout << "rotation : { " << data.rotation.x << data.rotation.y << data.rotation.z << data.rotation.w << " }\n";
		std::cout << "scale    : { " << data.scale.x << data.scale.y << data.scale.z << " }\n";
	}
	
	auto serializedString = SerializeContainer(deserializedVector);

	std::cout << serializedString << std::endl;

	return 0;
}
