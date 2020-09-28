#ifndef EBAYAPIABSTRACT_H
#define EBAYAPIABSTRACT_H

#include "EbayData.h"
#include "EbaySearch.h"

#include <QObject>


class QNetworkReply;
class EbayAPIManager;


enum class EbayApiType
{
	None,
	Finding,
	Shopping,
	Trading,
	Taxonomy
};


class EbayAPIAbstract :	public QObject
{
	Q_OBJECT
public:
	explicit EbayAPIAbstract(EbayApiType type, EbayAPIManager* manager, const QUrl & apiEndpoint, const QString & version, int apiCallsLimit, QObject *parent = nullptr);

	EbayAPIAbstract() = delete;
	EbayAPIAbstract(const EbayAPIAbstract &) = delete;
	EbayAPIAbstract& operator=(const EbayAPIAbstract &) = delete;

	virtual ~EbayAPIAbstract();

public: //API
	const QUrl endpoint() const;
	const QString version() const;
	int callsLimit() const;	

	EbayApiType type() const;
	static QString typeToString(EbayApiType type);

	QString errorMessage() const;

	QString lastCallAppId() const;
	void setLastCallAppId(const QString & appId);

public:
	QUrl currentRequestUrl() const;

	EbaySiteId siteId() const;	

	virtual QString siteIdToString(EbaySiteId siteId) const  = 0;

	QString siteIdToString() const;

	bool isRequestProcessed() const;

	EbayAPIManager* apiManager() const;

public:
	virtual size_t callsCount() const = 0;
	virtual void setCallsCount(size_t count) = 0;
	virtual void incrementCallsCount() = 0;
	virtual void resetCallsCount() = 0;

public slots:

	void setSiteId(EbaySiteId siteId);	

	virtual void setStatus(const QString & text, bool error = false);
	virtual void setProgress(int value, int total, bool primary = true, const QString & suffix = QString() );
	virtual void setError(const QString & message, bool critical = false);

signals:	

	void callsCountChanged(size_t count);

	void statusChanged(EbayApiType type, const QString & text, const QString & threadId, bool error);

	void progressChanged(int value, int total, bool primary, const QString & suffix);

	void apiLimitReached(const QString & appId, EbayApiType type);

protected:	

	virtual void initRequest();
	virtual void deinitRequest(QNetworkReply*& reply);

	QString replyError(QNetworkReply* reply) const;	

protected:

	struct RetryInfo
	{
		bool retryIsStarted() const 
		{
			return retriesCount_ > 0;
		}
		
		bool maxRetriesReached() const
		{
			return retriesCount_ >= retriesMaxCount_;
		}

		void reset()
		{
			currentError_.clear();
			retriesCount_ = 0;
		}

		QString        currentError_;
		int            retriesCount_ = 0;
		int            retriesMaxCount_ = 3;
		int            retryDelayMsec_ = 2000;
	};

protected:

	EbayAPIManager*         apiManager_;
	
	QNetworkRequest  		networkRequest_;

	const QUrl              endpoint_;
	const QString           version_;	

	const int				callsLimit_;	
	
	QUrl					currentRequestUrl_;

	EbaySiteId			    siteId_;	

	bool                    requestProcessed_;	

	QElapsedTimer           responseTimer_;
	int                     responseTimeMs_;

	EbayApiType             type_;

	QString                 errorMessage_;

	QByteArray              requestPostData_;

	RetryInfo               retryInfo_;

	QString                 lastCallAppId_;
};


#endif //EBAYAPIABSTRACT_H 

