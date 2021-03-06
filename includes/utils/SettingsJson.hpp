#pragma once

#include <map>
#include <string>
#include "json.hpp"
#include "Logging.hpp"

// TODO(tnicolas42) add lists in json loading

namespace JsonOpt {
	enum Enum {
		NO_OPT = 0,
		DISCARD_DISABLED = 1,
		VERBOSE = 2,
		COLOR = 4,
	};
}

template<class T>
class JsonObj {
	public:
		JsonObj() { init(); }
		explicit JsonObj(std::string const & name) { init(name); }
		JsonObj(std::string const & name, T const & val) : _value(val) { init(name); }
		virtual ~JsonObj() {}
		JsonObj(JsonObj const & src) { *this = src; }
		JsonObj & operator=(JsonObj const & rhs) {
			if (this != &rhs) {
				logDebug("WARNING -> JsonObj object copied");
				_name = rhs._name;
				_hasMin = rhs._hasMin;
				_min = rhs._min;
				_hasMax = rhs._hasMax;
				_max = rhs._max;
				_value = rhs._value;
			}
			return *this;
		}
		void init(std::string const & name = "") {
			_name = name;
			_description = "";
			_hasMin = false;
			_hasMax = false;
			_disableInFile = false;
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
		T &					getMax() { return _max; }
		T const &			getMax() const { return _max; }
		T &					getMin() { return _min; }
		T const &			getMin() const { return _min; }
		JsonObj<T> &		setDescription(std::string const & desc) { _description = desc; return *this; }
		JsonObj<T> &		disableInFile(bool disable = true) { _disableInFile = disable; return *this; }
		bool				isDisabledInFile() { return _disableInFile; }
		void				setName(std::string const & name) { _name = name; }
		std::string	&		getName() { return _name; }
		std::string	const &	getName() const { return _name; }
		std::string			getInfo() const {
			std::ostringstream out;
			out << "  // ";
			if (_description != "")
				out << _description << " ";
			out << "<";
			if (typeid(T) == typeid(std::string)) out << "string";
			else if (typeid(T) == typeid(int64_t)) out << "int64";
			else if (typeid(T) == typeid(uint64_t)) out << "uint64";
			else if (typeid(T) == typeid(double)) out << "double";
			else if (typeid(T) == typeid(bool)) out << "bool";
			if (_hasMin || _hasMax) {
				out << " range=[";
				if (_hasMin)
					out << _min;
				out << ":";
				if (_hasMax)
					out << _max;
				out << "]";
			}
			if (_disableInFile) {
				out << " [disabled in file]";
			}
			out << ">";
			return out.str();
		}

		friend std::ostream & operator<<(std::ostream & out, const JsonObj & jsonObj) {
			out << std::boolalpha << jsonObj.get();
			return out;
		}

	protected:
		std::string _name;
		std::string _description;
		T			_value;
		bool		_hasMin;
		T			_min;
		bool		_hasMax;
		T			_max;
		bool		_disableInFile;
};

class SettingsJson {
	public:
		SettingsJson();
		SettingsJson(SettingsJson const &src);
		virtual ~SettingsJson();

		SettingsJson &operator=(SettingsJson const &rhs);

		SettingsJson &	name(std::string const & name);
		SettingsJson &	description(std::string const & description);

		bool		loadFile(std::string const & filename);
		bool		loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp);
		void		saveToFile(std::string const & filename);
		std::string	toString(uint32_t opt = JsonOpt::NO_OPT) const;

		template<class T>
		JsonObj<T> &	add(std::string const & name) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return *(tmpMap[name]);
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T> *>(name, new JsonObj<T>(name)));
			return *(tmpMap[name]);
		}
		template<class T>
		JsonObj<T> &	add(std::string const & name, T val) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return *(tmpMap[name]);
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T> *>(name, new JsonObj<T>(name, val)));
			return *(tmpMap[name]);
		}
		template<class T>
		JsonObj<T> &	update(std::string const & name)  {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				return *(tmpMap[name]);
			}
			throw SettingsException("undefined setting " + name);
		}
		template<class T>
		T const &		get(std::string const & name) const {
			std::map<std::string, JsonObj<T> *> const & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name)->get();
			throw SettingsException("undefined setting " + name);
		}
		template<class T>
		T &				get(std::string const & name) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name)->get();
			throw SettingsException("undefined setting " + name);
		}
		std::string const &		s(std::string const & name) const { return get<std::string>(name); }
		std::string &			s(std::string const & name)  { return get<std::string>(name); }
		int64_t const &			i(std::string const & name) const { return get<int64_t>(name); }
		int64_t &				i(std::string const & name)  { return get<int64_t>(name); }
		uint64_t const &		u(std::string const & name) const { return get<uint64_t>(name); }
		uint64_t &				u(std::string const & name)  { return get<uint64_t>(name); }
		double const &			d(std::string const & name) const { return get<double>(name); }
		double &				d(std::string const & name)  { return get<double>(name); }
		bool const &			b(std::string const & name) const { return get<bool>(name); }
		bool &					b(std::string const & name)  { return get<bool>(name); }
		SettingsJson const &	j(std::string const & name) const { return get<SettingsJson>(name); }
		SettingsJson &			j(std::string const & name)  { return get<SettingsJson>(name); }

		class SettingsException : public std::runtime_error {
			public:
				SettingsException();
				explicit SettingsException(const char* what_arg);
				explicit SettingsException(const std::string what_arg);
		};

		std::map<std::string, JsonObj<std::string> *>	stringMap;  // s
		std::map<std::string, JsonObj<int64_t> *>		intMap;  // i
		std::map<std::string, JsonObj<uint64_t> *>		uintMap;  // u
		std::map<std::string, JsonObj<double> *>		doubleMap;  // f
		std::map<std::string, JsonObj<bool> *>			boolMap;  // b
		std::map<std::string, JsonObj<SettingsJson> *>	jsonMap;  // j

	private:
		std::string _name;
		std::string _description;

		template<class T>
		std::map<std::string, JsonObj<T> *> const & _getMap() const {
			if (typeid(T) == typeid(int64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&intMap);
			if (typeid(T) == typeid(int32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&intMap);
			if (typeid(T) == typeid(int16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&intMap);
			if (typeid(T) == typeid(int8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&intMap);
			if (typeid(T) == typeid(uint64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&uintMap);
			if (typeid(T) == typeid(uint32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&uintMap);
			if (typeid(T) == typeid(uint16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&uintMap);
			if (typeid(T) == typeid(uint8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&uintMap);
			if (typeid(T) == typeid(double))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&doubleMap);
			if (typeid(T) == typeid(float))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&doubleMap);
			if (typeid(T) == typeid(bool))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&boolMap);
			if (typeid(T) == typeid(std::string))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&stringMap);
			if (typeid(T) == typeid(SettingsJson))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&jsonMap);
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}
		template<class T>
		std::map<std::string, JsonObj<T> *> & _getMap() {
			if (typeid(T) == typeid(int64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&intMap);
			if (typeid(T) == typeid(int32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&intMap);
			if (typeid(T) == typeid(int16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&intMap);
			if (typeid(T) == typeid(int8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&intMap);
			if (typeid(T) == typeid(uint64_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&uintMap);
			if (typeid(T) == typeid(uint32_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&uintMap);
			if (typeid(T) == typeid(uint16_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&uintMap);
			if (typeid(T) == typeid(uint8_t))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&uintMap);
			if (typeid(T) == typeid(double))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&doubleMap);
			if (typeid(T) == typeid(float))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&doubleMap);
			if (typeid(T) == typeid(bool))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&boolMap);
			if (typeid(T) == typeid(std::string))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&stringMap);
			if (typeid(T) == typeid(SettingsJson))
				return *reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&jsonMap);
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}


		template<class T>
		void deleteMap(std::map<std::string, JsonObj<T> *> & map) {
			for (auto it = map.begin(); it != map.end(); it++) {
				delete it->second;
			}
			map.erase(map.begin(), map.end());
		}

		template<class T>
		std::map<std::string, JsonObj<T> *> copyMap(std::map<std::string, JsonObj<T> *> const & map) {
			std::map<std::string, JsonObj<T> *> cpy;
			for (auto it = map.begin(); it != map.end(); it++) {
				cpy.insert(std::pair<std::string, JsonObj<T> *>(it->first, new JsonObj<T>(*it->second)));
			}
			return cpy;
		}
};

std::ostream & operator<<(std::ostream & o, SettingsJson const & s);
