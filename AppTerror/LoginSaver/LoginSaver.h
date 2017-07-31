#ifndef TERROR_LOGIN_SAVER_H
#define TERROR_LOGIN_SAVER_H

#include "Config.h"

namespace terror {
	struct LoginSaver {
		friend class boost::serialization::access;

		LoginSaver(const std::string& saveFilename) 
			: saveFilename(saveFilename), login(""), password("") {}

		void save() {
			ofstream outputFile(this->saveFilename.c_str(), std::ios::out | std::ios::binary);
			serializeObject(*this, outputFile);
			outputFile.close();
		}

		bool tryLoad() {
			ifstream inputFile(this->saveFilename.c_str(), std::ios::in | std::ios::binary);
			if (!inputFile.is_open()) {
				return false;
			}
			try {
				deserializeObject(inputFile, *this);
				inputFile.close();
				return true;
			}catch (std::exception& e) {
				inputFile.close();
				this->clear();
				return false;
			}
		}

		bool loginFileExists() {
			return is_regular_file(this->saveFilename);
		}

		void clear() {
			this->login = "";
			this->password = "";
			remove(saveFilename);
		}

		std::string saveFilename;
		std::string login;
		std::string password;

	private:
		template<class Archive_>
		void save(Archive_& archive, const unsigned int version) const {
			string compressedLogin;
			string compressedPassword;
			compressString(this->login, compressedLogin);
			compressString(this->password, compressedPassword);
			archive & compressedLogin;
			archive & compressedPassword;
		}

		template<class Archive_>
		void load(Archive_& archive, const unsigned int version) {	
			string compressedLogin;
			string compressedPassword;
			archive & compressedLogin;
			archive & compressedPassword;
			decompressString(compressedLogin, this->login);
			decompressString(compressedPassword, this->password);
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()

	};

	typedef SharedPointer<LoginSaver> LoginSaverPtr; 
}

#endif