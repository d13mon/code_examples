#include "precompiled.h"

#include "UserSession.h"
#include "DatabaseManager.h"
#include "CamerasView.h"


//======================class UserSession==================================================================

QMap<QString, QVariant> UserSession::m_defaultSettings;

//---------------------------------------------------------------------------------------------------------
UserSession::UserSession(int defaultLanguageIndex, QObject *parent)
	: QObject(parent)
	, m_userId(-1)
	, m_valid(false)
{
	doInitDefaultSettings(defaultLanguageIndex);
}

//---------------------------------------------------------------------------------------------------------
UserSession::~UserSession()
{

}

//---------------------------------------------------------------------------------------------------------
void UserSession::doInitDefaultSettings(int defaultLanguageIndex)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	auto defPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#else
	auto defPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#endif
	
	m_defaultSettings["cs_language"] = defaultLanguageIndex;  
	m_defaultSettings["cs_dateformat"] = Qt::SystemLocaleShortDate;
	m_defaultSettings["cs_export_path"] = defPath;	
	m_defaultSettings["cs_playback_limit"] = 4;
	m_defaultSettings["cs_snapshot_path"] = defPath;

	m_defaultSettings["cs_alarm_timeout"] = 15;
	m_defaultSettings["cs_alarm_max_channels"] = 16;
	
	m_defaultSettings["cp_create_new_tab_when_current_full"] = false;

	m_defaultSettings["mw_cameras_action_mode"] = 0;

	m_defaultSettings["qp_btn_interval1"] = 15;
	m_defaultSettings["qp_btn_interval2"] = 30;
	m_defaultSettings["qp_btn_interval3"] = 60;

	m_defaultSettings["vs_aspect_ratio_1x1"] = true;
	m_defaultSettings["vs_enable_over_all_windows_mode"] = false;
	m_defaultSettings["vs_enable_statistics"] = false;
	m_defaultSettings["vs_show_camera"] = true;
	m_defaultSettings["vs_show_server"] = true;
	m_defaultSettings["vs_show_status"] = true;
	m_defaultSettings["vs_show_timestamp"] = true;
	m_defaultSettings["vs_show_add_tab_button"] = false;
	m_defaultSettings["vs_show_ptz_panel"] = false;
	m_defaultSettings["vs_streaming_mode"] = 0;
}

//---------------------------------------------------------------------------------------------------------
QString UserSession::toMD5Hash(const QString& source)
{	
	QCryptographicHash md5(QCryptographicHash::Md5);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	md5.addData(source.toAscii());
#else
	md5.addData(source.toLatin1());
#endif

	return md5.result().toHex().constData(); 
}

//---------------------------------------------------------------------------------------------------------
UserSessionLayout UserSession::sessionLayoutFromSettingsLayoutState(const CameraLayoutInfo& info)
{
	UserSessionLayout layout;

	layout.resolution = info.resolution;
	layout.name = info.name;
	layout.uuid = info.id;

	foreach (auto ucell, info.unitedCells)
	{
		UserSessionLayoutCell cell;

		cell.pos = ucell.first;
		cell.width = ucell.second.width();
		cell.height = ucell.second.height();

		layout.cells << cell;
	}

	return layout;
}

//------------------------------------------------------------------------------------------------------------------------------------------
CamerasViewSaveState UserSession::settingsViewStateFromSessionView(const UserSessionView & view)
{
	CamerasViewSaveState state;

	state.expandedCameraIndex = view.expandedCameraIndex;
	state.monitorType = static_cast<MonitorType>(view.monitorType);
	state.monitorScreenIndex = view.monitorScreenIndex;
	state.id = view.uuid;
	state.name = view.name;
	state.streamMode = CameraStreamMode::Live;

	state.layout = settingsLayoutStateFromSession(view.layout);

	foreach (auto cam, view.cameras)
	{
		CameraState camState;

		camState.index = cam.positionIndex;
		camState.streamMode;
		camState.zoom;	
		
		camState.channel.id       = boost::lexical_cast <Uuid> (cam.channelUuid.toStdString());
		camState.channel.endpoint = cam.endpoint.toStdString();	

	/*	LOG_DEBUG_X("DATABASE", "Restoring Channel: Id=%s | endpoint=%s | Data=%s | size=%s", camState.channel.id, 
			camState.channel.endpoint, cam.channelData.data(), cam.channelData.size ());*/

		if (!camState.channel.chan.ParseFromArray (cam.channelData, cam.channelData.size ()))
		{
			LOG_WARNING_X ("RESTORING_CHANNEL", "Can not restore channel %s information", cam.channelUuid.toStdString() );
			continue;
		}		

		camState.config;
		camState.selected = true;	

		state.cameras << camState;
	}

	return state;
}

//------------------------------------------------------------------------------------------------------------------------------------------
CameraLayoutInfo UserSession::settingsLayoutStateFromSession(const UserSessionLayout & lay)
{
	CameraLayoutInfo info;

	info.resolution = lay.resolution;
	info.name = lay.name;
	info.id = lay.uuid;

	foreach (auto cell, lay.cells)
	{
		UnitedCell ucell;
		ucell.first = cell.pos;
		ucell.second = QSize(cell.width, cell.height);

		info.unitedCells << ucell;
	}

	return info;
}


//================================OBSERVER STUFF==========================================================

//---------------------------------------------------------------------------------------------------------
void UserSession::registerObserver(UserSessionObserver* observer)
{
	m_observers.push_back(observer);
}

//---------------------------------------------------------------------------------------------------------
void UserSession::removeObserver(UserSessionObserver* observer)
{
	for(auto it = m_observers.begin(); it != m_observers.end(); ++it)
	{
		if (*it == observer)
		{
			m_observers.erase(it);
			return;
		}
	}	
}

//---------------------------------------------------------------------------------------------------------
void UserSession::notifyObservers(const QString & dbgFrom)
{
	Q_UNUSED(dbgFrom);

	LOG_DEBUG_X("DATABASE", "Session: Notify from %s", dbgFrom.toStdString());

	setValid(false);

	for(const auto & o: m_observers)
	{
		o->updateForUserSession(*this);
	}

	setValid(true);
}


//---------------------------------------------------------------------------------------------------------
bool UserSession::isValid() const
{
	return m_valid;
}

//---------------------------------------------------------------------------------------------------------
void UserSession::setValid(bool valid)
{
	m_valid = valid;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::login(const QString& username, bool autologin /*= true*/, const QString& password /*= QString() */)
{
	auto passHash = toMD5Hash(password);
	QString dbPass;
	if(DatabaseManager::getUserPassword(username, dbPass))
	{
		if(autologin ||  dbPass == passHash )
		{
			LOG_DEBUG_X("DATABASE", "Session: Logined '%d'", username.toStdString());

			doLoadUserData(username);			
			
			setValid(true);
			return true;
		}	
		else
		{
			LOG_INFO_X("LOGIN", "Login for user '%s' failed: \n%s <> %s(Valid)", username.toStdString(), passHash.toStdString(), dbPass.toStdString());
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
void UserSession::logout()
{	
	doClearUserData();	
	setValid(false);	
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::isLogged() const
{
	return m_userId != -1;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::isEmpty() const
{
	return !isLogged();
}

//---------------------------------------------------------------------------------------------------------
QString UserSession::username() const
{
	return m_userInfo.username;
}

//---------------------------------------------------------------------------------------------------------
int UserSession::userId() const
{
	return m_userInfo.id;
}

//---------------------------------------------------------------------------------------------------------
UserSessionInfo UserSession::userInfo() const
{
	return m_userInfo;
}

//---------------------------------------------------------------------------------------------------------
UserSessionRights UserSession::rights() const
{
	return m_userRights;
}

//---------------------------------------------------------------------------------------------------------
UserSessionDevicesList UserSession::devices() const
{
	//updateUserDevices(false);

	return m_userDevices;
}

//---------------------------------------------------------------------------------------------------------
UserSessionSettingsMap UserSession::settings() const
{
	return m_userSettings;
}

//---------------------------------------------------------------------------------------------------------
UserSessionLayoutsList UserSession::layouts() const
{
	return m_userLayouts;
}

//---------------------------------------------------------------------------------------------------------
QVariant UserSession::setting(const QString & name) const
{
	if(m_userSettings.contains(name))
	{
		return m_userSettings[name].value;
	}

	return QVariant();
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::isAdminLogged() const
{
	return username() == "admin";
}

//---------------------------------------------------------------------------------------------------------
void UserSession::update(bool notify)
{
	LOG_DEBUG_X("DATABASE", "Session: UPDATE ALL: notify=%d", notify);
	
	updateUserInfo();
	updateUserRights();	
	updateUserSettings();
	updateUserDevices();		
	updateUserLayouts();
	updateUserTabViews();
	updateUserBookmarkViews();

	if(notify)
	{
		notifyObservers("update");
	}	
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserInfo(bool notify /*= false*/)
{
	if(DatabaseManager::getUserInfo(m_userId, m_userInfo))
	{
		if(notify)
		{
			notifyObservers("updateUserInfo");
		}
	}	
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserRights(bool notify /*= false*/)
{
	if(DatabaseManager::getUserRights(m_userId, m_userRights))
	{
		if(notify)
		{
			notifyObservers("updateUserRights");
		}
	}	
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserDevices(bool notify /*= false*/)
{
	if(DatabaseManager::getUserDevices(m_userId, m_userDevices))
	{
		if(notify)
		{
			notifyObservers("updateUserDevices");
		}

	/*	for(int i=0; i < m_userDevices.count(); i++)
		{
			LOG_DEBUG_X("DATABASE", "Session: Load %s: Channels=%d ", m_userDevices[i].name.toStdString(), m_userDevices[i].channels.count());
		}*/
	}
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserSettings(bool notify /*= false*/)
{
	bool success  = false;
	if(!DatabaseManager::getUserSettings(m_userId, m_userSettings))
		return;

	if(!m_userSettings.count())
	{
		auto langIndex = m_defaultSettings["cs_language"].toInt();
		if(!DatabaseManager::addAllDefaultSettingsForUser(m_userId, langIndex))
			return;

		success = DatabaseManager::getUserSettings(m_userId, m_userSettings);

		BALTER_ASSERT(m_userSettings.count());
	}
	else
	{
		for(auto it = m_defaultSettings.begin(); it != m_defaultSettings.end(); ++it)
		{
			auto s = it.key();

			if(!m_userSettings.contains(s))
			{
				auto addedId = DatabaseManager::addDefaultUserSetting(m_userId, s, it.value());
				if(addedId != -1)
				{
					UserSessionSetting newSetting;
					newSetting.id = addedId;
					newSetting.name = s;
					newSetting.value = it.value();

					m_userSettings[s] = newSetting;
				}
				else
				{
					BALTER_ASSERT(0);
				}
			}
		}//for

		success = true;		
	}

	if(success)
	{
		if(notify)
		{
			notifyObservers("updateUserSettings");
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserLayouts(bool notify /*= false*/)
{
	bool success  = false;
	if(!DatabaseManager::getUserLayouts(m_userId, m_userLayouts))
		return;

	if(!m_userLayouts.count())
	{
		if(!DatabaseManager::addDefaultLayoutsForUser(m_userId))
			return;

		success = DatabaseManager::getUserLayouts(m_userId, m_userLayouts);

		BALTER_ASSERT(m_userLayouts.count());
	}
	else
	{
		success  = true;
	}


	if(success)
	{
		if(notify)
		{
			notifyObservers("updateUserLayouts");
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserTabViews(bool notify /*= false*/)
{
	if(DatabaseManager::getUserTabViews(m_userId, m_userTabViews))
	{
		if(notify)
		{
			notifyObservers("updateUserTabViews");
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void UserSession::updateUserBookmarkViews(bool notify /*= false*/)
{
	if(DatabaseManager::getUserBookmarkViews(m_userId, m_userBookmarkViews))
	{
		if(notify)
		{
			notifyObservers("updateUserBookmarkViews");
		}

		for(auto it = m_userBookmarkViews.begin(); it != m_userBookmarkViews.end(); ++it)
		{
			LOG_DEBUG_X("DATABASE", "Load View: %s: Chans=%d ", it->name.toStdString(), it->cameras.count());
		}
	}
}


//---------------------------------------------------------------------------------------------------------
void UserSession::doLoadUserData(const QString& username)
{
	if(username.isEmpty())
		return;

	DatabaseManager::getUserId(username, m_userId);		
	BALTER_ASSERT(m_userId != -1);
	
	updateUserDevices(false);
}

//---------------------------------------------------------------------------------------------------------
void UserSession::doClearUserData()
{
	LOG_DEBUG_X("DATABASE", "Session: Clear");
	
	m_userId = -1;
	m_userRights = UserSessionRights();
	m_userInfo =  UserSessionInfo();
	m_userDevices.clear();
	m_userSettings.clear();
	m_userLayouts.clear();
	m_userTabViews.clear();
	m_userBookmarkViews.clear();

	notifyObservers("UserSession::doClearUserData");
}

//==================================DEVICES=============================================================================

//---------------------------------------------------------------------------------------------------------
bool UserSession::channelRights(const QString & endpoint, const QString & uuid, UserSessionChannelInfo & rights) const
{
	for(int i=0; i < m_userDevices.size(); i++)
	{
		auto dev = m_userDevices[i];

		if(dev.getEndpoint() == endpoint)
		{
			if(dev.channels.contains(uuid))
			{
				rights = dev.channels[uuid];
			}
			else
			{
				rights = UserSessionChannelInfo();				
			}

			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::channelRights(const std::string & endpoint, const Uuid & uuid, UserSessionChannelInfo & rights) const
{
	return channelRights(QString::fromStdString(endpoint), QString::fromStdString (boost::lexical_cast <std::string> (uuid)), rights);
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::channelRights(const ChannelInfo & channel, UserSessionChannelInfo & rights) const
{
	return channelRights(channel.endpoint, channel.id, rights);
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::isDeviceHasNoAccess(const std::string& endpoint) const
{
	QString endp = QString::fromStdString(endpoint);

	for(int i=0; i < m_userDevices.size(); i++)
	{
		auto dev = m_userDevices[i];

		if(dev.getEndpoint() == endp)
		{			
			for(auto it = dev.channels.begin(); it != dev.channels.end(); ++it)
			{
				if(it.value().rightLive)
					return false;
			}
			
			return true;
		}
	}
		
	return true;
}

//==============================================LAYOUTS=====================================================================

//---------------------------------------------------------------------------------------------------------------------------
bool UserSession::addLayout(const UserSessionLayout& layout)
{
	if(DatabaseManager::addUserLayout(m_userId, true, layout))
	{
		updateUserLayouts(true);
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------------------------------------------------------
bool UserSession::updateLayout(const UserSessionLayout & layout)
{
	if(DatabaseManager::updateUserLayout(m_userId, layout))
	{
		updateUserLayouts(true);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::deleteLayout(const QUuid& uuid)
{
	if(DatabaseManager::deleteLayout(m_userId, uuid))
	{
		updateUserLayouts(true);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
UserSessionLayout UserSession::defaultLayout() const
{
	foreach (auto l, m_userLayouts)
	{
		if(l.resolution == 1)
		{
			return l;
		}
	}

	if(m_userLayouts.size())
	{
		return m_userLayouts[0];
	}

	return UserSessionLayout();
}


//=====================================VIEWS================================================================

//---------------------------------------------------------------------------------------------------------
UserSessionViewsMap UserSession::bookmarkViews() const
{
	return m_userBookmarkViews;
}

//---------------------------------------------------------------------------------------------------------
UserSessionViewsMap UserSession::tabViews() const
{
	return m_userTabViews;
}

//---------------------------------------------------------------------------------------------------------
std::vector<UserSessionView> UserSession::tabViewsAsList() const
{
	std::vector<UserSessionView> viewsList;

	for (auto it = m_userTabViews.begin(); it != m_userTabViews.end(); ++it)
	{
		viewsList.push_back(*it);
	}

	// Сортируем по возрастания индекса таба, чтобы восстановить в правильном порядке
	std::sort(viewsList.begin(), viewsList.end(), [](const UserSessionView & v1, const UserSessionView & v2)
	{
		return v1.viewIndex < v2.viewIndex;
	});

	return viewsList;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::getUserView(bool isBookmark, const QUuid & uuid, UserSessionView & view) const
{
	if(isBookmark)
	{
		if(m_userBookmarkViews.contains(uuid))
		{
			view = m_userBookmarkViews[uuid];
			return true;
		}
	}
	else
	{
		if(m_userTabViews.contains(uuid))
		{
			view = m_userTabViews[uuid];
			return true;
		}
	}	

	return false;		
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::getMonitorView(const QUuid& monUuid, UserSessionView & mon) const
{
	foreach(auto view, m_userTabViews)
	{
		if(view.uuid == monUuid)
		{
			BALTER_ASSERT(view.monitorType != static_cast<int>(MonitorType::None));

			mon = view;
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
int UserSession::getUserViewId(bool isBookmark, const QUuid & uuid) const
{
	UserSessionView view;
	if(getUserView(isBookmark, uuid, view))
	{
		return view.id;
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::addUserView(bool isBookmark, const CamerasViewSaveState& state)
{
	if(state.id.isNull())
		return false;

	if(isBookmark)
	{
		if(m_userBookmarkViews.contains(state.id))
			return false;
	}
	else
	{
		if(m_userTabViews.contains(state.id))
			return false;
	}

	LOG_DEBUG_X("DATABASE", "Session: ADD CAMVIEW: Bookmark=%d | Name=%s | Uuid=%s",  isBookmark, state.name.toStdString(), state.id.toString().toStdString() );

	if(DatabaseManager::addUserView(m_userId, isBookmark, state))
	{
		isBookmark ?  updateUserBookmarkViews(false) : updateUserTabViews(false);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::renameUserView(bool isBookmark, const QUuid & uuid, const QString & newName)
{
	auto viewId = getUserViewId(isBookmark, uuid);
	if (viewId == -1)
	{
		//LOG_DEBUG_X("VIEW_RENAME", "DIDN'T FOUND!!! Book=%d | Id=%s", isBookmark, uuid.toString().toStdString());
		return false;
	}

	//LOG_DEBUG_X("VIEW_RENAME", "CAMVIEW: Book=%d | uuid=%s | newName = %s | viewId=%d", isBookmark, uuid.toString().toStdString(), newName.toStdString(), viewId);

	LOG_DEBUG_X("DATABASE", "Session: RENAME CAMVIEW: Bookmark=%d | ViewId=%d | newName=%s | UUid=%s", isBookmark, viewId, newName.toStdString(),  uuid.toString().toStdString() );

	if(DatabaseManager::renameUserView(viewId, newName))
	{
		isBookmark ?  updateUserBookmarkViews(false) : updateUserTabViews(false);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::deleteUserView(bool isBookmark, const QUuid & uuid)
{
	auto viewId = getUserViewId(isBookmark, uuid);
	if(viewId == -1)
		return false;

	LOG_DEBUG_X("DATABASE", "Session: DELETE CAMVIEW: Bookmark=%d | ViewId=%d | Uuid=%s", isBookmark, viewId, uuid.toString().toStdString() );

	if(DatabaseManager::deleteUserView(viewId))
	{
		isBookmark ?  updateUserBookmarkViews(false) : updateUserTabViews(false);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::updateUserView(bool isBookmark, const QUuid& uuid, const CamerasViewSaveState& state)
{
	auto viewId = getUserViewId(isBookmark, uuid);
	if(viewId == -1)
		return false;

	LOG_DEBUG_X("DATABASE", "Session: UPDATE CAMVIEW: Bookmark=%d | ViewId=%d | name=%s | Uuid=%s", isBookmark, viewId, state.name.toStdString(), uuid.toString().toStdString() );

	if(DatabaseManager::updateUserView(viewId, state))
	{
		isBookmark ?  updateUserBookmarkViews(false) : updateUserTabViews(false);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------
bool UserSession::setUserCurrentViewIndex(int index)
{
	if (DatabaseManager::setUserCurrentViewIndex(m_userId, index))
	{	
		updateUserInfo(false);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------------
bool UserSession::setUserViewIndex(const QUuid& uuid, int index)
{
	auto viewId = getUserViewId(false, uuid);
	if (viewId == -1)
		return false;

	if (DatabaseManager::setUserViewIndex(viewId, index))
	{
		updateUserTabViews(false);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------------
QMap<QString, QVariant> UserSession::defaultSettings()
{
	return m_defaultSettings;
}




































//==============================================Struct UserSessionInfo=======================================================
//---------------------------------------------------------------------------------------------------------------------------
UserSessionInfo::UserSessionInfo()
	: id(0)
	, autologin(false)
	, currentViewIndex(-1)
{
}

UserSessionInfo::UserSessionInfo(const QString& name, const QString& pass, const QString& email_, int id_ /*= -1*/)
	: id(id_)
	, username(name)
	, passHash(pass)
	, email(email_)
	, autologin(false)
	, currentViewIndex(-1)
{

}



//=========================================Struct UserSessionRights=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionRights::UserSessionRights()	
{

}



//===================Struct UserSessionCommonRights=======================================================
//---------------------------------------------------------------------------------------------------------
UserSessionCommonRights::UserSessionCommonRights()
	: devicesPermission(false)
	, playbackPermission(false)
	, exportPermission(false)
	, userSetupPermission(false)
	, guiSetupPermission(false)
	, clientSetupPermission(false)
	, multiMonitorPermission(false)
	, audioPermission(false)
	, ptzPermission(false)
	, tabsPermission(false)
{

}



//=========================================Struct UserSessionChannelInfo=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionChannelInfo::UserSessionChannelInfo()
	: id(-1)
	,  rightLive(false)
	, rightPlayback(false)
	, rightExport(false)
	, rightAudio(false)
	, rightPtz(false)	
{

}




//=========================================Struct UserSessionDeviceInfo=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionDeviceInfo::UserSessionDeviceInfo()
	: devId(-1)
	, port(0)
	, connectionMethod(0)
	, detectionMethod(0)
{

}

QString UserSessionDeviceInfo::getEndpoint() const
{
	return connectionMethod == static_cast<int>(DeviceConnectionMethod::IP)
		? "tcp://" + address + ":" + QString("%1").arg(port)
		: "p2p://" + p2pid + ":" + QString("%1").arg(port);
}



//=========================================Struct UserSessionSetting=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionSetting::UserSessionSetting()
	: id(-1)
{

}

//=========================================Struct UserSessionLayoutCell=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionLayoutCell::UserSessionLayoutCell()
	:id(-1)
	, width(0)
	, height(0)
{

}

//-------------------------------------------------------------------------------------------------------------------------------
UserSessionLayoutCellInfo UserSessionLayoutCell::info()
{
	return UserSessionLayoutCellInfo(pos, QSize(width, height));
}

//=========================================Struct UserSessionLayout=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionLayout::UserSessionLayout()
	: id(-1)
	, resolution(0)
{

}

//-------------------------------------------------------------------------------------------------------------------------------
UserSessionLayoutCellInfoList UserSessionLayout::cellsInfo() const
{
	UserSessionLayoutCellInfoList list;

	foreach (auto c, cells)
	{
		list << c.info();
	}

	return list;
}

//=========================================Struct UserSessionViewCamera=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionViewCamera::UserSessionViewCamera()
	: id(-1)
	, positionIndex(-1)	
{

}

//=========================================Struct UserSessionView=======================================================
//-------------------------------------------------------------------------------------------------------------------------------
UserSessionView::UserSessionView()
	: id(-1)
	, isBookmark(false)
	, monitorType(-1)
	, monitorScreenIndex(-1)
	, expandedCameraIndex(-1)	
	, viewIndex(-1)	
{

}
