#include "precompiled.h"

#include "EbayAPIAbstract.h"
#include "EbayAPIManager.h"


//------------------------------------------------------------------------------------------------------------------------------------------
EbayAPIAbstract::EbayAPIAbstract(EbayApiType type, EbayAPIManager* manager, const QUrl & apiEndpoint, const QString & version, int apiCallsLimit, QObject *parent/* = nullptr*/)
	: QObject(parent)
	, siteId_(EbaySiteId::EBAY_DE)
	, apiManager_(manager)	
	, type_(type)
	, requestProcessed_(false)
	, callsLimit_(apiCallsLimit)
	, endpoint_(apiEndpoint)
	, version_(version)
	, responseTimeMs_(0)	
{

	if (APP::isCompMode())
	{
		retryInfo_.retriesMaxCount_ = 2;
	}

	connect(manager, &EbayAPIManager::siteIdChanged, this, &EbayAPIAbstract::setSiteId);

	connect(this, &EbayAPIAbstract::progressChanged, manager, &EbayAPIManager::setProgress, Qt::QueuedConnection);
	connect(this, SIGNAL(statusChanged(EbayApiType, const QString &, const QString &, bool)), manager,
		SLOT(setStatus(EbayApiType, const QString &, const QString &, bool)), Qt::QueuedConnection);
	connect(this, &EbayAPIAbstract::apiLimitReached, manager, &EbayAPIManager::setApiLimitReached);
}

//------------------------------------------------------------------------------------------------------------------------------------------
EbayAPIAbstract::~EbayAPIAbstract()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------
const QUrl EbayAPIAbstract::endpoint() const
{
	return endpoint_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
int EbayAPIAbstract::callsLimit() const
{
	return callsLimit_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
EbayApiType EbayAPIAbstract::type() const
{
	return type_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
QString EbayAPIAbstract::typeToString(EbayApiType type)
{
	switch (type)
	{
	case EbayApiType::None: return "eBay API";	
	case EbayApiType::Finding: return "Finding API";
	case EbayApiType::Shopping:	return "Shopping API";
	case EbayApiType::Trading: return "Trading API";
	case EbayApiType::Taxonomy:	return "Taxonomy API";
	default:
		Q_ASSERT(0);
		break;
	}

	return QString();
}

//------------------------------------------------------------------------------------------------------------------------------------------
QString EbayAPIAbstract::errorMessage() const
{
	return errorMessage_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
QString EbayAPIAbstract::lastCallAppId() const
{
	return lastCallAppId_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::setLastCallAppId(const QString & appId)
{
	lastCallAppId_ = appId;
}

//------------------------------------------------------------------------------------------------------------------------------------------
const QString EbayAPIAbstract::version() const
{
	return version_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
QUrl EbayAPIAbstract::currentRequestUrl() const
{
	return currentRequestUrl_;
}
//------------------------------------------------------------------------------------------------------------------------------------------
EbaySiteId EbayAPIAbstract::siteId() const
{
	return siteId_;
}
//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::setSiteId(EbaySiteId siteId)
{
	if (siteId_ != siteId)
	{
		siteId_ = siteId;		
	}	
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::setStatus(const QString & text, bool error /*= false*/)
{
	emit statusChanged(type_, text, APP::getThreadIdAsQString(), error);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::setProgress(int value, int total, bool primary /*= true*/, const QString & suffix /*= QString() */)
{
	if (value > total)
		total = value;

	emit progressChanged(value, total, primary, suffix);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::setError(const QString & message, bool critical)
{
	Q_UNUSED(critical);

	errorMessage_ = message;

	emit statusChanged(type_, tr("ERROR: ") + message, APP::getThreadIdAsQString(), true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
QString EbayAPIAbstract::siteIdToString() const
{
	return siteIdToString(siteId_);
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool EbayAPIAbstract::isRequestProcessed() const
{
	return requestProcessed_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
EbayAPIManager* EbayAPIAbstract::apiManager() const
{
	return apiManager_;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::initRequest()
{
	requestProcessed_ = true;

	responseTimer_.start();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void EbayAPIAbstract::deinitRequest(QNetworkReply*& reply)
{		
	requestProcessed_ = false;	

	responseTimeMs_ = responseTimer_.elapsed();

	if (reply)
	{
		reply->deleteLater();		
	}	
}

//------------------------------------------------------------------------------------------------------------------------------------------
QString EbayAPIAbstract::replyError(QNetworkReply* reply) const
{
	return QString("(%1)%2").arg(reply->error()).arg(reply->errorString());
}
