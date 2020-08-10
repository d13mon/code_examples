#include "precompiled.h"

#include "EbayData.h"

#include <QJsonArray>



//=================================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------------
EbayCategoryTreeNode::EbayCategoryTreeNode()
	:categoryTreeNodeLevel_(-1)
{

}

//---------------------------------------------------------------------------------------------------------------------------------
void EbayCategoryTreeNode::read(const QJsonObject & json)
{
	if (json.contains("categoryTreeNodeLevel") && json["categoryTreeNodeLevel"].isDouble())
		categoryTreeNodeLevel_ = json["categoryTreeNodeLevel"].toInt(-1);

	if (json.contains("category"))
	{
		auto category = json["category"].toObject();

		if (category.contains("categoryId") && category["categoryId"].isString())
			categoryId_ = category["categoryId"].toString();

		if (category.contains("categoryName") && category["categoryName"].isString())
			categoryName_ = category["categoryName"].toString();	
		
	}		
	
	int jsonArraySize = 0;
	if (json.contains("childCategoryTreeNodes") && json["childCategoryTreeNodes"].isArray())
	{		
		auto jsonNodes = json["childCategoryTreeNodes"].toArray();
		jsonArraySize = jsonNodes.size();	

		childNodes_.clear();

		for (int i = 0; i < jsonNodes.size(); ++i)
		{
			auto jsonNode = jsonNodes[i].toObject();

			EbayCategoryTreeNode node;			
			node.read(jsonNode);
			childNodes_.insert(i, node);
		}
	}	
	
//	qDebug() << "EbayCategoryTreeNode:READ: " << categoryId_ << " | " << categoryName_ << " | " << categoryTreeNodeLevel_ << " | Children= " << jsonArraySize;

}

//=================================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------------
EbayCredentials::EbayCredentials(const QString & appId, const QString & certId, const QString & devId, const QString & ruName)
	: appId_(appId)
	, certId_(certId)
	, devId_(devId)
	, ruName_(ruName)
{

}

//---------------------------------------------------------------------------------------------------------------------------------
bool EbayCredentials::operator<(const EbayCredentials & other) const
{
	return appId_ < other.appId_;
}

//---------------------------------------------------------------------------------------------------------------------------------
bool EbayCredentials::operator==(const EbayCredentials & other) const
{
	return appId_ == other.appId_;
}

