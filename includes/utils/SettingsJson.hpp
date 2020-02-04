#pragma once

#include <map>
#include <string>
#include "json.hpp"
#include "Logging.hpp"

// TODO(tnicolas42) add lists in json loading

template<class T>
class JsonObj {
	public:
		JsonObj() : _name(""), _hasMin(false), _hasMax(false) {}
		explicit JsonObj(std::string const & name) : _name(name), _hasMin(false), _hasMax(false) {}
		JsonObj(std::string const & name, T val) : _name(name), _value(val), _hasMin(false), _hasMax(false) {}
		virtual ~JsonObj() {}
		JsonObj(JsonObj const & src) { *this = src; }
		JsonObj & operator=(JsonObj const & rhs) {
			if (this != &rhs) {
				_name = rhs._name;
				_hasMin = rhs._hasMin;
				_min = rhs._min;
				_hasMax = rhs._hasMax;
				_max = rhs._max;
				_value = rhs._value;
			}
			return *this;
		}

		T const &		get() const { return _value; }
		T &				get() { return _value; }
		JsonObj<T> &	setValue(T value) {
			if (_hasMin && value < _min) {
				logWarn("unable to set arg " << _name << ": " << value << " is less than min value (" << _min << ")");
				return *this;
			}
			if (_hasMax && value > _max) {
				logWarn("unable to set arg " << _name << ": " << value << " is greater than max value (" << _max << ")");
				return *this;
			}
			_value = value;
			return *this;
		}
		bool			checkValue(T value) const {
			if (_hasMin && value < _min) {
				return false;
			}
			if (_hasMax && value > _max) {
				return false;
			}
			return true;
		}
		JsonObj<T> &		setMin(T value) { _hasMin = true; _min = value; return *this; }
		JsonObj<T> &		setMax(T value) { _hasMax = true; _max = value; return *this; }
		void				setName(std::string const & name) { _name = name; }
		std::string	&		getName() { return _name; }
		std::string	const &	getName() const { return _name; }

		friend std::ostream & operator<<(std::ostream & out, const JsonObj & jsonObj) {
			out << std::boolalpha << jsonObj.get();
			return out;
		}

	protected:
		std::string _name;
		T			_value;
		bool		_hasMin;
		T			_min;
		bool		_hasMax;
		T			_max;
};

class SettingsJson {
	public:
		SettingsJson();
		SettingsJson(SettingsJson const &src);
		virtual ~SettingsJson();

		SettingsJson &operator=(SettingsJson const &rhs);

		bool		loadFile(std::string const & filename);
		bool		loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp);
		void		saveToFile(std::string const & filename);
		std::string	toString() const;

		template<class T>
		JsonObj<T> &	add(std::string name) {
			std::map<std::string, JsonObj<T>> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return tmpMap[name];
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T>>(name, JsonObj<T>(name)));
			return tmpMap[name];
		}
		template<class T>
		JsonObj<T> &	add(std::string name, T val) {
			std::map<std::string, JsonObj<T>> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return tmpMap[name];
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T>>(name, JsonObj<T>(name, val)));
			return tmpMap[name];
		}
		template<class T>
		JsonObj<T> &	update(std::string name)  {
			std::map<std::string, JsonObj<T>> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				return tmpMap[name];
			}
			throw SettingsException("undefined setting " + name);
		}
		template<class T>
		T const &		get(std::string name) const {
			std::map<std::string, JsonObj<T>> const & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name).get();
			throw SettingsException("undefined setting " + name);
		}
		template<class T>
		T &				get(std::string name) {
			std::map<std::string, JsonObj<T>> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name).get();
			throw SettingsException("undefined setting " + name);
		}
		std::string const &		s(std::string name) const { return get<std::string>(name); }
		std::string &			s(std::string name)  { return get<std::string>(name); }
		int64_t const &			i(std::string name) const { return get<int64_t>(name); }
		int64_t &				i(std::string name)  { return get<int64_t>(name); }
		uint64_t const &		u(std::string name) const { return get<uint64_t>(name); }
		uint64_t &				u(std::string name)  { return get<uint64_t>(name); }
		double const &			d(std::string name) const { return get<double>(name); }
		double &				d(std::string name)  { return get<double>(name); }
		bool const &			b(std::string name) const { return get<bool>(name); }
		bool &					b(std::string name)  { return get<bool>(name); }
		SettingsJson const &	j(std::string name) const { return get<SettingsJson>(name); }
		SettingsJson &			j(std::string name)  { return get<SettingsJson>(name); }

		class SettingsException : public std::runtime_error {
			public:
				SettingsException();
				explicit SettingsException(const char* what_arg);
				explicit SettingsException(const std::string what_arg);
		};

		std::map<std::string, JsonObj<std::string>>		stringMap;  // s
		std::map<std::string, JsonObj<int64_t>>			intMap;  // i
		std::map<std::string, JsonObj<uint64_t>>		uintMap;  // u
		std::map<std::string, JsonObj<double>>			doubleMap;  // f
		std::map<std::string, JsonObj<bool>>			boolMap;  // b
		std::map<std::string, JsonObj<SettingsJson>>	jsonMap;  // j

	private:
		template<class T>
		std::map<std::string, JsonObj<T>> const & _getMap() const {
			if (typeid(T) == typeid(int64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&intMap);
			if (typeid(T) == typeid(int32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&intMap);
			if (typeid(T) == typeid(int16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&intMap);
			if (typeid(T) == typeid(int8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&intMap);
			if (typeid(T) == typeid(uint64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&uintMap);
			if (typeid(T) == typeid(uint32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&uintMap);
			if (typeid(T) == typeid(uint16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&uintMap);
			if (typeid(T) == typeid(uint8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&uintMap);
			if (typeid(T) == typeid(double))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&doubleMap);
			if (typeid(T) == typeid(float))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&doubleMap);
			if (typeid(T) == typeid(bool))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&boolMap);
			if (typeid(T) == typeid(std::string))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&stringMap);
			if (typeid(T) == typeid(SettingsJson))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> const *>(&jsonMap);
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}
		template<class T>
		std::map<std::string, JsonObj<T>> & _getMap() {
			if (typeid(T) == typeid(int64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&intMap);
			if (typeid(T) == typeid(int32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&intMap);
			if (typeid(T) == typeid(int16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&intMap);
			if (typeid(T) == typeid(int8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&intMap);
			if (typeid(T) == typeid(uint64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&uintMap);
			if (typeid(T) == typeid(uint32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&uintMap);
			if (typeid(T) == typeid(uint16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&uintMap);
			if (typeid(T) == typeid(uint8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&uintMap);
			if (typeid(T) == typeid(double))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&doubleMap);
			if (typeid(T) == typeid(float))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&doubleMap);
			if (typeid(T) == typeid(bool))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&boolMap);
			if (typeid(T) == typeid(std::string))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&stringMap);
			if (typeid(T) == typeid(SettingsJson))
				return *reinterpret_cast<std::map<std::string, JsonObj<T>> *>(&jsonMap);
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}
};

std::ostream & operator<<(std::ostream & o, SettingsJson const & s);
