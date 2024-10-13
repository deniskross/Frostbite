#pragma once
#ifndef ENGINE_H__
#define ENGINE_H__

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <functional>

//CONSTANS

constexpr float EPSILON = 0.00001f;

typedef int index_t;

template<typename Cont>
index_t isize(const Cont& cont)
{
	return (index_t)cont.size();
}

typedef int id_t;
constexpr id_t ID_UNDEF = 0;

int random(int lo, int hi);


// RENDERING SUBSYSTEM

void vtWriteChar(int ch);
void vtCharAttr(char color);
void vtCursorPrevLine(int nLines);
void vtEraseInDisplay();
void vtOpenWnd();
void vtCloseWnd();

void writef(const char* fmt);



template<typename T, typename ...Args>
void writef(const char* fmt, T arg1, Args... args)
{
	while (*fmt) {
		if (*fmt == '{' && *(fmt + 1) == '}') {
#ifdef _DEBUG
			std::cout << arg1;
#endif //_DEBUG
			fmt += 2;
			writef(fmt, args...);
			return;
		}
		else {
			vtWriteChar(*fmt);
			fmt += 1;
		}
	}
	std::cout << std::flush;
}

// INPUT SUBSYSTEM

bool readln(std::string& input);


// DIALOG SUBSYSTEM

index_t dialog(
	const std::string& message,
	const std::string& prompt,
	const std::vector<std::string>& choices,
	const std::vector<std::string>& systemChoices);


//REGISTRY SUBSYSTEM

template<typename T>
class Registry {
public:
	const T* getDef(id_t id) const;
	void loadFromStream(std::istream& is);
	int count() const;

private:
	std::vector<T> items_;
};


template<typename T>
const T* Registry<T>::getDef(id_t id) const
{
	assert(id >= 0 && id < isize(items_));
	return &items_[id];
}

template<typename T>
void Registry<T>::loadFromStream(std::istream& is) {

	items_.clear();

	int countItems = 0;
	is >> countItems;

	T emptyDef;
	emptyDef.id = 0;
	items_.push_back(std::move(emptyDef));


	for (int j = 0; j < countItems; ++j) {
		T itemDef;
		is >> itemDef;
		items_.push_back(std::move(itemDef));
	}

}

template<typename T>
int Registry<T>::count() const
{
	return isize(items_);
}

//TABLE SUBSYSTEM

template<typename T>
class Table {
public:
	Table();
	const T* get(id_t id) const;
	int generate(const std::function <void(T&)>& init);
	int count() const;

private:
	std::vector<T> items_;
};


template<typename T>
Table<T> ::Table()
{
	items_.push_back(std::move(T()));
}

template<typename T>
const T* Table<T>::get(id_t id) const
{
	assert(id >= 0 && id < isize(items_));
	return &items_[id];
}

template<typename T>
int Table<T>::generate(const std::function <void(T&)>& init)
{
	id_t nextId = isize(items_);
	T item;
	init(item);
	item.id = nextId;
	items_.push_back(std::move(item));
	return nextId;
}


template<typename T>
int Table<T>::count() const
{
	return isize(items_);
}

#endif //ENGINE_H__