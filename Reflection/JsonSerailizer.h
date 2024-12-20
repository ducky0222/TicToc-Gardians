#pragma once
#include <string>

template <typename T>
concept SerializableContainer = requires(const T & container) {
	{ nlohmann::json(container) } -> std::convertible_to<nlohmann::json>;
};

template <typename Container>
std::string SerializeContainer(const Container& myContainer) requires SerializableContainer<Container> {
	nlohmann::json jsonObject = myContainer;
	return jsonObject.dump(2);
}

template <typename Container>
Container DeserializeContainerFromFile(const std::string& filename) {
	std::ifstream inputFile(filename);
	nlohmann::json jsonObject;
	inputFile >> jsonObject;
	inputFile.close();
	return jsonObject.get<Container>();
}
