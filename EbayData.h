#ifndef EBAYDATA_H
#define EBAYDATA_H

#include <QString>
#include <QJsonObject>
#include <QList>
#include <QMap>

#include <map>
#include <set>


enum class EbaySiteId {
	EBAY_US = 0,
	EBAY_UK = 3,
	EBAY_DE = 77
};


struct EbayCredentials
{
	EbayCredentials(const QString & appId = QString(), const QString & certId = QString(), 
		const QString & devId = QString(), const QString & ruName = QString());

	QString appId_;
	QString certId_;
	QString devId_;
	QString ruName_;

	bool operator==(const EbayCredentials & other) const;
	bool operator<(const EbayCredentials & other) const;
};

using CredentialsStatus = std::pair<EbayCredentials, int>;

/**
*   struct EbayCategoryTreeNode - узловой элемент дерева каталогов eBay
*/
struct EbayCategoryTreeNode
{
	EbayCategoryTreeNode();

	QString      categoryId_;
	QString      categoryName_;
	int          categoryTreeNodeLevel_;

	/**
	*  –екурсивный метод чтени¤ и парсинга данных из JSON
	*/
	void read(const QJsonObject & json);

	QList<EbayCategoryTreeNode> childNodes_;
};

using EbayCategoryTreeList = QList<EbayCategoryTreeNode>;


#endif //EBAYDATA_H
