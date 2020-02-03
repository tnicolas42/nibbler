#pragma once

#include <map>
#include <string>
#include "json.hpp"
#include "Logging.hpp"

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
			out << jsonObj.get();
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

		bool	loadFile(std::string const & filename);
		bool	loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp);

		// int
		JsonObj<int64_t> &	addi(std::string name, int64_t val = 0);
		JsonObj<int64_t> &	updatei(std::string name);
		int64_t				geti(std::string name) const;
		int64_t				i(std::string name) const;
		// uint
		JsonObj<uint64_t> &	addu(std::string name, uint64_t val = 0);
		JsonObj<uint64_t> &	updateu(std::string name);
		uint64_t			getu(std::string name) const;
		uint64_t			u(std::string name) const;
		// double
		JsonObj<double> &	addf(std::string name, double val = 0.0);
		JsonObj<double> &	updatef(std::string name);
		double				getf(std::string name) const;
		double				f(std::string name) const;
		// bool
		JsonObj<bool> &	addb(std::string name, bool val = false);
		JsonObj<bool> &	updateb(std::string name);
		bool			getb(std::string name) const;
		bool			b(std::string name) const;
		// string
		JsonObj<std::string> &	adds(std::string name, std::string const & val = "");
		JsonObj<std::string> &	updates(std::string name);
		std::string const &		gets(std::string name) const;
		std::string &			gets(std::string name);
		std::string const &		s(std::string name) const;
		std::string &			s(std::string name);
		// json
		SettingsJson &			addj(std::string name);
		SettingsJson &			updatej(std::string name);
		SettingsJson const &	getj(std::string name) const;
		SettingsJson &			getj(std::string name);
		SettingsJson const &	j(std::string name) const;
		SettingsJson &			j(std::string name);

		class SettingsException : public std::runtime_error {
			public:
				SettingsException();
				explicit SettingsException(const char* what_arg);
				explicit SettingsException(const std::string what_arg);
		};

		std::map<std::string, JsonObj<int64_t>>			intMap;  // i
		std::map<std::string, JsonObj<uint64_t>>		uintMap;  // u
		std::map<std::string, JsonObj<double>>			doubleMap;  // f
		std::map<std::string, JsonObj<bool>>			boolMap;  // b
		std::map<std::string, JsonObj<std::string>>		stringMap;  // s
		std::map<std::string, JsonObj<SettingsJson>>	jsonMap;  // j
};

std::ostream & operator<<(std::ostream & o, const SettingsJson & s);
