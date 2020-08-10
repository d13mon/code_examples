#ifndef USERSESSION_H
#define USERSESSION_H


struct CamerasViewSaveState;
struct CameraLayoutInfo;
class UserSessionObserver; 
typedef std::shared_ptr <UserSessionObserver> UserSessionObserverPtr;
typedef std::weak_ptr <UserSessionObserver> UserSessionObserverWeakPtr;


/**
 * @class UserSessionInfo
 * @brief Информация о пользователе
 */
struct UserSessionInfo
{
	UserSessionInfo();
	UserSessionInfo(const QString& name, const QString& pass, const QString& email_, int id_ = -1);

	int			id;
	QString		username;
	QString		passHash;
	QString		email;
	bool        autologin;
	int         currentViewIndex;
};


/**
 * @class UserSessionCommonRights
 * @brief Групповые(общие) права пользователя
 */
struct UserSessionCommonRights
{
	UserSessionCommonRights();

	bool devicesPermission;
	bool playbackPermission;
	bool exportPermission;
	bool userSetupPermission;
	bool guiSetupPermission;
	bool clientSetupPermission;
	bool multiMonitorPermission;
	bool audioPermission;
	bool ptzPermission;
	bool tabsPermission;
};

/**
 * @class UserSessionChannelInfo
 * @brief Информация о канале в сессии пользователя
 */
struct UserSessionChannelInfo
{	
	UserSessionChannelInfo();	

	int			id;
	QString		uuid;
	QString		name;
	bool        rightLive;
	bool        rightPlayback;
	bool		rightExport;
	bool		rightAudio;
	bool		rightPtz;
};

typedef QMap<QString, UserSessionChannelInfo> UserSessionChannelsMap;

/**
 * @class UserSessionDeviceInfo
 * @brief  Информация о девайсе в сессии пользователя
 */
struct UserSessionDeviceInfo
{	
	UserSessionDeviceInfo();

	QString getEndpoint() const;

	int			devId;
	QString		name;
	QString		address;
	int			port;	
	QString		p2pid;
	QString		login;
	QString		pass;
	int			connectionMethod;
	int			detectionMethod;

	UserSessionChannelsMap channels;
};

typedef QList<UserSessionDeviceInfo> UserSessionDevicesList;

/**
 * @class UserSessionRights
 * @brief Права пользователя
 */
struct UserSessionRights
{
	UserSessionRights();
	
	UserSessionCommonRights common;	
};


/**
 * @class UserSessionSetting
 * @brief Опция настройки пользователя
 */
struct UserSessionSetting
{
	UserSessionSetting();
	
	int id;
	QString name;
	QVariant value;	
};

typedef QMap<QString, UserSessionSetting> UserSessionSettingsMap;

typedef QPair<QPoint, QSize> UserSessionLayoutCellInfo;

/**
 * @class UserSessionLayoutCell
 * @brief Ячейка разбивки пользователя
 */
struct UserSessionLayoutCell
{
	UserSessionLayoutCell();

	UserSessionLayoutCellInfo info();
	
	int id;
	QPoint pos;
	int width;
	int height;
};

typedef QList<UserSessionLayoutCell> UserSessionLayoutCellsList;

typedef QList<UserSessionLayoutCellInfo> UserSessionLayoutCellInfoList;


/**
 * @class UserSessionLayout
 * @brief Разбивка пользователя
 */
struct UserSessionLayout
{
	UserSessionLayout();

	UserSessionLayoutCellInfoList cellsInfo() const;
	
	int id;
	QUuid uuid;
	QString name;
	int resolution;
	UserSessionLayoutCellsList cells;
};

typedef QList<UserSessionLayout> UserSessionLayoutsList;


/**
 * @class UserSessionViewCamera
 * @brief Информация о камере на табе
 */
struct UserSessionViewCamera
{
	UserSessionViewCamera();
	
	int id;
	int positionIndex;	
	QString channelUuid;
	QString endpoint;
	QByteArray channelData;	
};

typedef QList<UserSessionViewCamera> UserSessionViewCamerasList;


/**
 * @class UserSessionView
 * @brief Таб пользователя
 */
struct UserSessionView
{
	UserSessionView();
	
	int id;
	QUuid uuid;
	QString name;
	bool isBookmark;
	int monitorType;
	int monitorScreenIndex;
	int expandedCameraIndex;
	UserSessionLayout layout;	
	UserSessionViewCamerasList cameras;	
	int viewIndex;
};


typedef QMap<QUuid, UserSessionView> UserSessionViewsMap;


/**
 * @class UserSession
 * @brief Сессия пользователя
 */
class UserSession : public QObject
{
	Q_OBJECT

public:
	explicit UserSession(int defaultLanguageIndex, QObject *parent = 0);
	virtual ~UserSession();

public: //CONVERSIONS
	static QString toMD5Hash(const QString& source);

	static UserSessionLayout sessionLayoutFromSettingsLayoutState(const CameraLayoutInfo& info);

	static CamerasViewSaveState settingsViewStateFromSessionView(const UserSessionView & view);
	static CameraLayoutInfo settingsLayoutStateFromSession( const UserSessionLayout & lay);
	

public: //LOGIN
	/**
	*  @brief 
	*/
	bool login(const QString& username, bool autologin = true, const QString& password = QString() );

	/**
	*  @brief 
	*/
	void logout();

	/**
	*  @brief 
	*/
	bool isLogged() const;

	/**
	*  @brief 
	*/
	bool isAdminLogged() const;
	
	/**
	*  @brief То же, что и !IsLogged
	*/
	bool isEmpty() const;

	/**
	*  @brief Сессия валидная?
	*/
	bool isValid() const;
	void setValid(bool valid);

public slots:
	
	void updateUserInfo(bool notify = false);	
	void updateUserRights(bool notify  = false);
	void updateUserDevices(bool notify = false);	
	void updateUserSettings(bool notify = false);	
	void updateUserLayouts(bool notify = false);	
	void updateUserTabViews(bool notify = false);
	void updateUserBookmarkViews(bool notify = false);
	
	void update(bool notify);

public: //USERS

	/**
	*  @brief Имя залогиненного юзера
	*/
	QString username() const;

	/**
	*  @brief Информация о залогиненном пользователе	
	*/
	UserSessionInfo userInfo() const;

	/**
	*  @brief Права залогиненного пользователя	
	*/
	UserSessionRights rights() const;

	/**
	*  @brief Список девайсов (dvr) залогиненного пользователя		
	*/
	UserSessionDevicesList devices() const;
	
	/**
	*  @brief Список сеттингов залогиненного пользователя		
	*/
	UserSessionSettingsMap settings() const;

	/**
	*  @brief Список разбивок залогиненного пользователя		
	*/
	UserSessionLayoutsList layouts() const;

	/**
	*  @brief Получение значения конкретной настройки	
	*/
	QVariant setting(const QString & name) const;

	/**
	*  @brief Список вьюсов (закладок)		
	*/
	UserSessionViewsMap bookmarkViews() const;

	/**
	*  @brief Список вьюсов (табов)		
	*/
	UserSessionViewsMap tabViews() const;

	/**
	*  @brief Для удобства в виде простого списка
	*/
	std::vector<UserSessionView> UserSession::tabViewsAsList() const;

	/**
	*  @brief Id юзера в БД
	*/
	int userId() const;

	/**
	*  @brief Дефолтные сеттинги на этом клиенте
	*/
	static QMap<QString, QVariant> defaultSettings();

private: //USER

	static void doInitDefaultSettings(int defaultLanguageIndex);

public: //DEVICES

	/**
	*  @brief Канальные права пользователя на конкретный канал
	*/
	bool channelRights(const QString & endpoint, const QString& uuid, UserSessionChannelInfo & rights) const;
	bool channelRights(const std::string & endpoint, const Uuid & uuid, UserSessionChannelInfo & rights) const;
	bool channelRights(const ChannelInfo & channel, UserSessionChannelInfo & rights) const;

	/**
	*  @brief Есть ли у пользователя доступ к девайсу
	*
	*  Если все каналы девайса не имеют права Live, то доступа нет
	*/
	bool isDeviceHasNoAccess(const std::string& endpoint) const;

public: //LAYOUTS

	bool addLayout(const UserSessionLayout& layout);
	bool updateLayout( const UserSessionLayout & layout);
	bool deleteLayout(const QUuid& uuid);
	UserSessionLayout defaultLayout() const; 

public: //VIEWS

	bool addUserView(bool isBookmark, const CamerasViewSaveState& state);

	bool getUserView(bool isBookmark, const QUuid & uuid, UserSessionView & view) const; 
	int getUserViewId(bool isBookmark, const QUuid & uuid) const; 
	bool getMonitorView (const QUuid& monUuid, UserSessionView & mon) const;

	bool renameUserView(bool isBookmark, const QUuid & uuid, const QString & newName);
	bool deleteUserView(bool isBookmark, const QUuid & uuid);

	bool updateUserView(bool isBookmark, const QUuid& uuid, const CamerasViewSaveState& state);

	bool setUserCurrentViewIndex(int index);

	bool setUserViewIndex(const QUuid& uuid, int index);
	
public: //Observer stuff
	void registerObserver(UserSessionObserver* observer);	
	void removeObserver(UserSessionObserver* observer);
	void notifyObservers(const QString & dbgFrom);

private:
	void doLoadUserData(const QString& username);
	void doClearUserData();	

private:

	std::vector<UserSessionObserver*> m_observers;
	
	int							 m_userId;
	UserSessionInfo				 m_userInfo;
	UserSessionRights			 m_userRights;
	UserSessionDevicesList		 m_userDevices;
	UserSessionSettingsMap		 m_userSettings;
	UserSessionLayoutsList       m_userLayouts;
	UserSessionViewsMap			 m_userTabViews;
	UserSessionViewsMap			 m_userBookmarkViews;

	static QMap<QString, QVariant> m_defaultSettings;

	bool						 m_valid;
};

typedef std::shared_ptr <UserSession> UserSessionPtr;
typedef std::weak_ptr <UserSession> UserSessionWeakPtr;



/**
 * @class UserSessionObserver
 * @brief Интерфейс наблюдателя сессии пользователя
 */
class UserSessionObserver 
{
public:
	virtual void updateForUserSession(const UserSession& session) = 0;
};



#endif // USERSESSION_H
