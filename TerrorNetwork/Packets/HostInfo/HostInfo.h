#ifndef TERROR_NETWORK_HOST_INFO_H
#define TERROR_NETWORK_HOST_INFO_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	typedef PointerList<UserInfo> UserInfoPtrList;

	class HostInfo : public Sendable {
	public:
		friend class boost::serialization::access;

		HostInfo(int oldHostId = -1) : oldHostId(oldHostId) {}

		virtual int getLabel() const {
			return HOST_INFO;
		}

		int getOldHostId() {
			return this->oldHostId;
		}

		void addUserInfo(UserInfoPtr userInfo) {
			this->users.add(userInfo);
		}

		UserInfoPtrList getUsers() {
			return this->users;
		}

		virtual ~HostInfo() {}

	protected:
		int oldHostId;
		UserInfoPtrList users;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->oldHostId;
			archive & this->users;
		}
	};

	typedef SharedPointer<HostInfo> HostInfoPtr;
}

#endif