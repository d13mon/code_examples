#ifndef USERSESSION_H
#define USERSESSION_H


struct CamerasViewSaveState;
struct CameraLayoutInfo;
class UserSessionObserver; 
typedef std::shared_ptr <UserSessionObserver> UserSessionObserverPtr;
typedef std::weak_ptr <UserSessionObserver> UserSessionObserverWeakPtr;


/**
 * @class UserSessionInfo
 * @brief ���������� � ������������
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
 * @brief ���������(�����) ����� ������������
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
 * @brief ���������� � ������ � ������ ������������
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
 * @brief  ���������� � ������� � ������ ������������
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
 * @brief ����� ������������
 */
struct UserSessionRights
{
	UserSessionRights();
	
	UserSessionCommonRights common;	
};


/**
 * @class UserSessionSetting
 * @brief ����� ��������� ������������
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
 * @brief ������ �������� ������������
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
 * @brief �������� ������������
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
 * @brief ���������� � ������ �� ����
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
 * @brief ��� ������������
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
 * @brief ������ ������������
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
	*  @brief �� ��, ��� � !IsLogged
	*/
	bool isEmpty() const;

	/**
	*  @brief ������ ��������?
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
	*  @brief ��� ������������� �����
	*/
	QString username() const;

	/**
	*  @brief ���������� � ������������ ������������	
	*/
	UserSessionInfo userInfo() const;

	/**
	*  @brief ����� ������������� ������������	
	*/
	UserSessionRights rights() const;

	/**
	*  @brief ������ �������� (dvr) ������������� ������������		
	*/
	UserSessionDevicesList devices() const;
	
	/**
	*  @brief ������ ��������� ������������� ������������		
	*/
	UserSessionSettingsMap settings() const;

	/**
	*  @brief ������ �������� ������������� ������������		
	*/
	UserSessionLayoutsList layouts() const;

	/**
	*  @brief ��������� �������� ���������� ���������	
	*/
	QVariant setting(const QString & name) const;

	/**
	*  @brief ������ ������ (��������)		
	*/
	UserSessionViewsMap bookmarkViews() const;

	/**
	*  @brief ������ ������ (�����)		
	*/
	UserSessionViewsMap tabViews() const;

	/**
	*  @brief ��� �������� � ���� �������� ������
	*/
	std::vector<UserSessionView> UserSession::tabViewsAsList() const;

	/**
	*  @brief Id ����� � ��
	*/
	int userId() const;

	/**
	*  @brief ��������� �������� �� ���� �������
	*/
	static QMap<QString, QVariant> defaultSettings();

private: //USER

	static void doInitDefaultSettings(int defaultLanguageIndex);

public: //DEVICES

	/**
	*  @brief ��������� ����� ������������ �� ���������� �����
	*/
	bool channelRights(const QString & endpoint, const QString& uuid, UserSessionChannelInfo & rights) const;
	bool channelRights(const std::string & endpoint, const Uuid & uuid, UserSessionChannelInfo & rights) const;
	bool channelRights(const ChannelInfo & channel, UserSessionChannelInfo & rights) const;

	/**
	*  @brief ���� �� � ������������ ������ � �������
	*
	*  ���� ��� ������ ������� �� ����� ����� Live, �� ������� ���
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
 * @brief ��������� ����������� ������ ������������
 */
class UserSessionObserver 
{
public:
	virtual void updateForUserSession(const UserSession& session) = 0;
};



#endif // USERSESSION_H
