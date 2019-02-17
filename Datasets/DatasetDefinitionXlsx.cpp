#include <math.h>
#include <memory>

#include <QDebug>
#include <QDomDocument>
#include <QVariant>
#include <QApplication>

#include "Shared/Logger.h"
#include "Common/Constants.h"
#include "Common/ProgressBar.h"

#include "DatasetDefinitionXlsx.h"

DatasetDefinitionXlsx::DatasetDefinitionXlsx(const QString& name,
                                             QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name, zipFileName)
{

}

bool DatasetDefinitionXlsx::getSheetList(QuaZip& zip)
{
	QMap<QString, QString> sheetIdToUserFriendlyNameMap;

    if( zip.setCurrentFile("xl/workbook.xml") )
	{
        //Open file in archive.
		QuaZipFile zipFile(&zip);
		if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + zipFile.getFileName() + ".");
			return false;
		}

        //Create, set content and read DOM.
		QDomDocument xmlDocument(__FUNCTION__);
		if ( false == xmlDocument.setContent(zipFile.readAll()) )
		{
            LOG(LOG_IMPORT_EXPORT, "File is corrupted.");
			return false;
		}
		zipFile.close();

		QDomElement root = xmlDocument.documentElement();
		QDomNodeList sheetNodes = root.firstChildElement("sheets").childNodes();

		if ( sheetNodes.size() <= 0 )
		{
            LOG(LOG_IMPORT_EXPORT, "File is corrupted, no sheets in xml.");
			return false;
		}

		for(int i = 0; i < sheetNodes.size(); ++i )
		{
			QDomElement sheet = sheetNodes.at(i).toElement();

			if ( false == sheet.isNull() )
			{
                sheetIdToUserFriendlyNameMap[sheet.attribute("r:id")] =
                    sheet.attribute("name");
			}
		}
	}
	else
	{
        LOG(LOG_IMPORT_EXPORT, "Can not open xl/workbook.xml file.");
		return false;
	}

	if(zip.setCurrentFile("xl/_rels/workbook.xml.rels"))
	{
        //Open file in archive.
		QuaZipFile zipFile(&zip);
		if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + zipFile.getFileName() + ".");
			return false;
		}

        //Create, set content and read DOM.
		QDomDocument xmlDocument(__FUNCTION__);
		if ( false == xmlDocument.setContent(zipFile.readAll()) )
		{
            LOG(LOG_IMPORT_EXPORT, "File is corrupted.");
			return false;
		}
		zipFile.close();

		QDomElement root = xmlDocument.documentElement();
		QDomNodeList sheetNodes = root.childNodes();

		if ( sheetNodes.size() <=0 )
		{
            LOG(LOG_IMPORT_EXPORT, "File is corrupted, no sheets in xml.");
			return false;
		}

		for(int i = 0; i < sheetNodes.size(); ++i )
		{
			QDomElement sheet = sheetNodes.at(i).toElement();

			if ( false == sheet.isNull() )
			{
				QMap<QString, QString>::const_iterator iterator =
						sheetIdToUserFriendlyNameMap.find(sheet.attribute("Id"));

				if(sheetIdToUserFriendlyNameMap.end() != iterator)
				{
                    sheetToFileMapInZip_[*iterator] =
                        "xl/" + sheet.attribute("Target");
				}
			}
		}
	}
	else
	{
        LOG(LOG_IMPORT_EXPORT,
            "No file named xl/_rels/workbook.xml.rels in archive.");
		return false;
	}

	//qDebug() << sheetToFileMapInZip_;

	return true;
}

bool DatasetDefinitionXlsx::loadStyles(QuaZip& zip)
{
	dateStyles_.clear();
	allStyles_.clear();

    //List of predefind excel styles for dates.
	dateStyles_<< 14 << 15 << 16 << 17 << 22;

    //Load styles.
	if(zip.setCurrentFile("xl/styles.xml"))
	{
        //Open file in archive.
		QuaZipFile zipFile(&zip);
		if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
            LOG(LOG_IMPORT_EXPORT, "Can not open file.");
			return false;
		}

        //Create, set content and read DOM.
		QDomDocument xmlDocument(__FUNCTION__);
		if ( false == xmlDocument.setContent(zipFile.readAll()) )
		{
            LOG(LOG_IMPORT_EXPORT, "Xml file is corrupted.");
			return false;
		}
		zipFile.close();

		QDomElement root = xmlDocument.documentElement();
		QDomNodeList sheetNodes = root.firstChildElement("numFmts").childNodes();

		for(int i = 0; i < sheetNodes.size(); ++i )
		{
			QDomElement sheet = sheetNodes.at(i).toElement();

			if ( sheet.hasAttribute("numFmtId") &&
				 sheet.hasAttribute("formatCode") )//&&
				 //sheet.attribute("formatCode").contains("@"))
			{
				QString formatCode = sheet.attribute("formatCode");
				bool gotD = formatCode.contains("d", Qt::CaseInsensitive);
				bool gotM = formatCode.contains("m", Qt::CaseInsensitive);
				bool gotY = formatCode.contains("y", Qt::CaseInsensitive);

				if ( (gotD && gotY) || (gotD && gotM) || (gotM && gotY) )
				{
					dateStyles_.push_back(sheet.attribute("numFmtId").toInt());
				}
			}
		}


		sheetNodes = root.firstChildElement("cellXfs").childNodes();

		for(int i = 0; i < sheetNodes.size(); ++i )
		{
			QDomElement sheet = sheetNodes.at(i).toElement();

			if ( false == sheet.isNull() &&
				 sheet.hasAttribute("numFmtId") )
			{
				allStyles_.push_back(sheet.attribute("numFmtId").toInt());
			}
		}
	}
	else
	{
        LOG(LOG_IMPORT_EXPORT, "No file named xl/workbook.xml in archive.");
		return false;
	}

	return true;
}


bool DatasetDefinitionXlsx::loadSharedStrings(QuaZip& zip)
{
    //Loading shared strings, it is seperate file in archive with uniqe table of
    //all strings, in spreadsheet there are calls to this table.
    if( zip.setCurrentFile("xl/sharedStrings.xml") )
	{
        //Set variable.
		QuaZipFile zipFile(&zip);

        //Opening file.
		if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + zipFile.getFileName() + ".");
			return false;
		}

		QXmlStreamReader xmlStreamReader;
		xmlStreamReader.setDevice(&zipFile);

        while( false == xmlStreamReader.atEnd() )
		{
			xmlStreamReader.readNext();

            //If 't' tag found add value to shared strings.
			if (xmlStreamReader.name() == "t")
			{
                stringsMap_[xmlStreamReader.readElementText()] = nextIndex_;
                nextIndex_++;
			}
		}
		zipFile.close();
	}
	else
	{
        LOG(LOG_IMPORT_EXPORT, "No file xl/sharedStrings.xml in archive.");
		return true;
	}

	return true;
}

bool DatasetDefinitionXlsx::getColumnList(QuaZip& zip,
										  const QString& sheetName)
{
    //Loading column names is using Excell names names. Set 600 temporary.
	columnsCount_ = 600;
    Constants::generateExcelColumnNames(excelColNames_, columnsCount_);

	if ( zip.setCurrentFile(sheetName) )
	{
		QuaZipFile zipFile(&zip);

        //Opening file.
		if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + zipFile.getFileName() + ".");
			return false;
		}

		QXmlStreamReader xmlStreamReader;
		xmlStreamReader.setDevice(&zipFile);

        //Variable with actual type of data in cell (s, str, null).
		QString currentColType = "s";
		QXmlStreamReader::TokenType lastToken = QXmlStreamReader::StartElement;

        //Go to first row.
        while ( false == xmlStreamReader.atEnd() &&
                xmlStreamReader.name() != "sheetData" )
		{
			xmlStreamReader.readNext();
		}

		xmlStreamReader.readNext(); xmlStreamReader.readNext();

        //Actual column number, from 0.
        int column = -1;
		lastToken = xmlStreamReader.tokenType();

        //Parse first row.
		while ( false == xmlStreamReader.atEnd() && xmlStreamReader.name() != "row" )
		{
            //If we encounter start of cell content we add it to list.
			if ( xmlStreamReader.name().toString() == QString("c") &&
                 xmlStreamReader.tokenType() == QXmlStreamReader::StartElement )
			{
                column++;
                QString rowNumber(xmlStreamReader.attributes().value("r").toString());
                static const QRegExp regExp("[0-9]");

                //If cells are missing add default name.
                while ( excelColNames_.indexOf(rowNumber.remove(regExp)) > column )
				{
					headerColumnNames_ << emptyColName_;
                    column++;
				}
                //Remember column type.
				currentColType = xmlStreamReader.attributes().value("t").toString();
			}

            //If we encounter start of cell content than add it to list.
			if ( false == xmlStreamReader.atEnd() &&
				 xmlStreamReader.name().toString() == QString("v") &&
				 xmlStreamReader.tokenType() == QXmlStreamReader::StartElement )
			{
				if(currentColType == "s")
				{
                    int value = xmlStreamReader.readElementText().toInt() + 1;
                    headerColumnNames_.push_back(stringsMap_.key(value));
				}
				else
				{
					if(currentColType == "str")
					{
						headerColumnNames_.push_back(xmlStreamReader.readElementText());
					}
					else
					{
						headerColumnNames_.push_back(xmlStreamReader.readElementText());
					}
				}
			}

            //If we encounter empty cell than add it to list.
			if ( xmlStreamReader.name().toString() == QString("c") &&
				 xmlStreamReader.tokenType() == QXmlStreamReader::EndElement &&
				 lastToken == QXmlStreamReader::StartElement )
			{
				headerColumnNames_ << emptyColName_;
			}
			lastToken = xmlStreamReader.tokenType();
			xmlStreamReader.readNext();
		}
	}
	else
	{
        LOG(LOG_IMPORT_EXPORT,
            "File named " + sheetName + " not found in archive.");
		return false;
	}

	return true;
}

bool DatasetDefinitionXlsx::openZipAndMoveToSecondRow(QuaZip& zip,
                                                      const QString& sheetName,
                                                      QuaZipFile& zipFile,
                                                      QXmlStreamReader& xmlStreamReader )
{
    //Open archive.
	if ( false == zip.setCurrentFile(sheetName) )
	{
        LOG(LOG_IMPORT_EXPORT,
            "File named " + sheetName + " not found in archive.");
		return false;
	}
	zipFile.setZip(&zip);
	if ( false == zipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
        LOG(LOG_IMPORT_EXPORT, "Can not open file " + zipFile.getFileName() + ".");
		return false;
	}

	xmlStreamReader.setDevice(&zipFile);

	bool secondRow = false;
	while ( false == xmlStreamReader.atEnd() )
	{
		if( xmlStreamReader.name() == "row" &&
			xmlStreamReader.tokenType() == QXmlStreamReader::StartElement )
		{
			if(true == secondRow)
				break;
			secondRow = true;
		}
		xmlStreamReader.readNextStartElement();
	}

	return true;
}

bool DatasetDefinitionXlsx::getColumnTypes( QuaZip& zip,
											const QString& sheetName )
{
    Constants::generateExcelColumnNames(excelColNames_, columnsCount_);

    ProgressBar bar(ProgressBar::PROGRESS_TITLE_DETECTING_COLUMN_TYPES, 0, nullptr);
	QTime performanceTimer;
	performanceTimer.start();

	QApplication::processEvents();

	QuaZipFile zipFile;
	QXmlStreamReader xmlStreamReader;

	if ( false == openZipAndMoveToSecondRow(zip, sheetName, zipFile, xmlStreamReader) )
	{
		return false;
	}

	columnsFormat_.clear();

	for ( int i = 0; i < columnsCount_; ++i )
	{
		columnsFormat_.push_back(DATA_FORMAT_UNKNOWN);
	}

    //Current column.
	int column = -1;

    //Current row.
	int rowCounter = 0;

	int charsToChopFromEndInCellName = 1;

	QXmlStreamAttributes xmlStreamAtrributes;

    //Optimization.
	static const QString rowTag("row");
	static const QString cellTag("c");
	static const QString sheetDataTag("sheetData");
	static const QString sTag("s");
	static const QString strTag("str");
	static const QString rTag("r");
	static const QString tTag("t");
	static const QString vTag("v");

	while ( false == xmlStreamReader.atEnd() &&
			0 != xmlStreamReader.name().compare(sheetDataTag) )
	{
        //If start of row encountered than reset column counter add
        //increment row counter.
		if ( 0 == xmlStreamReader.name().compare(rowTag) &&
			 true == xmlStreamReader.isStartElement() )
		{
			column = -1;

			if ( 0 == rowCounter % 100 )
			{
				QApplication::processEvents();
			}

			rowCounter++;

			double power = pow(10, charsToChopFromEndInCellName);
			if ( power <= rowCounter+1 )
			{
				charsToChopFromEndInCellName++;
			}
		}

        //When we encounter start of cell description.
		if ( 0 == xmlStreamReader.name().compare(cellTag) &&
			 true == xmlStreamReader.isStartElement() )
		{
			column++;

			QString stringToChop = xmlStreamReader.attributes().value(rTag).toString();
            int numberOfCharsToRemove =
                stringToChop.size() - charsToChopFromEndInCellName;
			int expectedIndexCurrentColumn =
                excelColNames_.indexOf(stringToChop.left(numberOfCharsToRemove));

            //If cells are missing increment column number.
			while ( expectedIndexCurrentColumn > column )
			{
				column++;
			}

            //If we encounter column outside expected grid we move to row end.
			if ( -1 == expectedIndexCurrentColumn )
			{
				xmlStreamReader.skipCurrentElement();
				continue;
			}

            //If data format in column is unknown than read it.
			if ( columnsFormat_.at(column) == DATA_FORMAT_UNKNOWN )
			{
				xmlStreamAtrributes = xmlStreamReader.attributes();
				QString value = xmlStreamAtrributes.value(tTag).toString();
				bool valueIsSTag = (0 == value.compare(sTag));
				if ( true == valueIsSTag ||
					 0 == value.compare(strTag) )
				{
					if ( true == valueIsSTag )
					{
						xmlStreamReader.readNextStartElement();

						if ( 0 == xmlStreamReader.name().compare(vTag) &&
							 xmlStreamReader.tokenType() == QXmlStreamReader::StartElement &&
                             true == stringsMap_.key(xmlStreamReader.readElementText().toInt() + 1).isEmpty() )
						{

						}
						else
						{
							columnsFormat_[column] = DATA_FORMAT_STRING;
						}
					}
					else
					{
						columnsFormat_[column] = DATA_FORMAT_STRING;
					}
				}
				else
				{
					QString value = xmlStreamAtrributes.value(sTag).toString();

					if ( false == value.isEmpty() &&
						 dateStyles_.contains(allStyles_.at(value.toInt())) )
					{
						columnsFormat_[column] = DATA_FORMAT_DATE;
					}
					else
					{
						columnsFormat_[column] = DATA_FORMAT_FLOAT;
					}
				}
			}
			else
			{
				if ( DATA_FORMAT_STRING != columnsFormat_.at(column) )
				{
                    //If type of column is known than check if it is correct.
					xmlStreamAtrributes = xmlStreamReader.attributes();
					QString value = xmlStreamAtrributes.value(tTag).toString();
					bool valueIsSTag = (0 == value.compare(sTag));
					if ( true == valueIsSTag || 0 == value.compare(strTag) )
					{
						if ( true == valueIsSTag )
						{
							xmlStreamReader.readNextStartElement();

							if ( 0 == xmlStreamReader.name().compare(vTag) &&
								 xmlStreamReader.tokenType() == QXmlStreamReader::StartElement &&
                                 true == stringsMap_.key(xmlStreamReader.readElementText().toInt() + 1).isEmpty() )
							{

							}
							else
							{
								columnsFormat_[column] = DATA_FORMAT_STRING;
							}
						}
						else
						{
							columnsFormat_[column] = DATA_FORMAT_STRING;
						}
					}
					else
					{
						QString value = xmlStreamAtrributes.value(sTag).toString();

						if ( false == value.isEmpty() &&
							 dateStyles_.contains(allStyles_.at(value.toInt())) )
						{
							if( columnsFormat_[column] != DATA_FORMAT_DATE )
							{
								columnsFormat_[column] = DATA_FORMAT_STRING;
							}
						}
						else
						{
							if ( columnsFormat_[column] != DATA_FORMAT_FLOAT )
							{
								columnsFormat_[column] = DATA_FORMAT_STRING;
							}
						}
					}
				}
			}
		}

		xmlStreamReader.readNextStartElement();
	}

	for ( int i = 0; i < columnsCount_; ++i )
	{
		if( DATA_FORMAT_UNKNOWN == columnsFormat_.at(i) )
		{
			columnsFormat_[i] = DATA_FORMAT_STRING;
		}
	}

	rowsCount_ = rowCounter;

    LOG(LOG_IMPORT_EXPORT, "Analyzed file having " + QString::number(rowsCount_) +
                           " rows in time " +
                           QString::number(performanceTimer.elapsed()*1.0/1000) +
						   " seconds.");

	return true;
}

bool DatasetDefinitionXlsx::getDataFromZip( QuaZip& zip,
											const QString& sheetName,
											QVector<QVector<QVariant> >* dataContainer,
											bool fillSamplesOnly )
{
    std::unique_ptr<ProgressBar> bar;

	if ( false == fillSamplesOnly )
	{
        bar.reset(new ProgressBar(ProgressBar::PROGRESS_TITLE_LOADING,
                                  rowsCount_,
                                  nullptr));
	}

	QApplication::processEvents();

	QTime performanceTimer;
	performanceTimer.start();

	QuaZipFile zipFile;
	QXmlStreamReader xmlStreamReader;

	openZipAndMoveToSecondRow(zip, sheetName, zipFile, xmlStreamReader);

    //Actual column number.
	int column = -1;

    //Actual data type in cell (s, str, null).
	QString currentColType = "s";

    //Actual row number.
	int rowCounter = 0;

    //Null elements row.
	QVector<QVariant> templateDataRow;

	QMap<int, int> activeColumnsMapping = QMap<int, int>();

	int columnToFill = 0;

	templateDataRow.resize(true == fillSamplesOnly ? columnsCount_ : getActiveColumnCount());
	for( int i = 0; i < columnsCount_; ++i )
	{
		if ( true == fillSamplesOnly || true == activeColumns_[i] )
		{
            templateDataRow[columnToFill] =
                getDefaultVariantForFormat(columnsFormat_[i]);
			activeColumnsMapping[i] = columnToFill;
			columnToFill++;
		}
	}

    //Protection from potential core related to empty rows.
	int containerSize = dataContainer->size();
	for ( int i = 0; i < containerSize; ++i )
	{
		(*dataContainer)[i] = templateDataRow;
	}

    //Current row data.
    QVector<QVariant> currentDataRow(templateDataRow);

    //Nmebr of chars to chop from end.
	int charsToChopFromEndInCellName = 1;

    //Optimization.
	static const QString rowTag("row");
	static const QString cellTag("c");
	static const QString sheetDataTag("sheetData");
	static const QString sTag("s");
	static const QString vTag("v");
	static const QString rTag("r");
	static const QString tTag("t");

	while ( false == xmlStreamReader.atEnd() &&
			0 != xmlStreamReader.name().compare(sheetDataTag) &&
			rowCounter <= rowsCount_ )
	{
        //If start of row encountered than reset column counter add
        //increment row counter.
        if ( 0 == xmlStreamReader.name().compare(rowTag) &&
			 true == xmlStreamReader.isStartElement() )
		{
			column = -1;

			if( 0 != rowCounter )
			{
				(*dataContainer)[rowCounter-1] = currentDataRow;
				currentDataRow = QVector<QVariant>(templateDataRow);

				double power = pow(10, charsToChopFromEndInCellName);
				if ( qRound(power) <= rowCounter+2 )
				{
					charsToChopFromEndInCellName++;
				}
			}

			rowCounter++;

			if ( false == fillSamplesOnly )
			{
                bar->updateProgress(rowCounter);
			}

			if( true == fillSamplesOnly && rowCounter > containerSize )
			{
				break;
			}
		}

        //When we encounter start of cell description.
		if ( 0 == xmlStreamReader.name().compare(cellTag) &&
			 true == xmlStreamReader.isStartElement() )
		{
			column++;

            QString stringToChop =
                xmlStreamReader.attributes().value(rTag).toString();
			int expectedIndexCurrentColumn =
					excelColNames_.indexOf(stringToChop.left(stringToChop.size() - charsToChopFromEndInCellName));

            //If cells missing than increment column number.
			while ( expectedIndexCurrentColumn > column )
			{
				column++;
			}

            //If we encounter column outside expected grid we move to row end.
			if ( -1 == expectedIndexCurrentColumn )
			{
				xmlStreamReader.skipCurrentElement();
				continue;
			}

            //Remember cell type.
			currentColType = xmlStreamReader.attributes().value(tTag).toString();
		}

        //Insert data into table.
        if ( false == xmlStreamReader.atEnd() &&
			 0 == xmlStreamReader.name().compare(vTag) &&
			 xmlStreamReader.tokenType() == QXmlStreamReader::StartElement &&
			 ( true == fillSamplesOnly || ( false == fillSamplesOnly && true == activeColumns_[column] ) ) )
		{
            DataFormat format = columnsFormat_.at(column);

            switch(format)
            {
                case DATA_FORMAT_STRING:
                {
                    //Strings.
                    if ( 0 == currentColType.compare(sTag) )
                    {
                        currentDataRow[activeColumnsMapping[column]] =
                            QVariant(xmlStreamReader.readElementText().toInt() + 1);
                    }
                    else
                    {
                        currentDataRow[activeColumnsMapping[column]] =
                            QVariant(getStringIndex(xmlStreamReader.readElementText()));
                    }
                    break;
                }

                case DATA_FORMAT_DATE:
                {
                    QDate startOfTheExcelWorld(1899, 12, 30);

                    //If YYYY-MM-DD HH:MI:SS cut and left YYYY-MM-DD.
                    int daysToAdd =
                        static_cast<int>(xmlStreamReader.readElementText().toDouble());
                    currentDataRow[activeColumnsMapping[column]] =
                        QVariant(startOfTheExcelWorld.addDays(daysToAdd));
                    break;
                }

                case DATA_FORMAT_FLOAT:
                {
                    if ( 0 != currentColType.compare(sTag) )
                    {
                        currentDataRow[activeColumnsMapping[column]] =
                            QVariant(xmlStreamReader.readElementText().toDouble());
                    }

                    break;
                }

                default:
                {
                    Q_ASSERT(false);
                    break;
                }
            }
		}
		xmlStreamReader.readNextStartElement();
	}

	if( 0 != rowCounter &&
		(false == fillSamplesOnly || (true == fillSamplesOnly && sampleSize_ > rowsCount_)) )
	{
		Q_ASSERT(rowCounter <= rowsCount_);
		if ( rowCounter <= rowsCount_ )
		{
			(*dataContainer)[rowCounter-1] = currentDataRow;
		}
	}

	if ( false == fillSamplesOnly )
	{
		Q_ASSERT(rowsCount_ == containerSize);

		rebuildDefinitonUsingActiveColumnsOnly();

        LOG(LOG_IMPORT_EXPORT, "Loaded file having " +
                               QString::number(rowsCount_) +
                               " rows in time " +
                               QString::number(performanceTimer.elapsed()*1.0/1000) +
                               " seconds.");
	}

	return true;
}

const QString& DatasetDefinitionXlsx::getSheetName()
{
	return sheetToFileMapInZip_.begin().value();
}

bool DatasetDefinitionXlsx::loadSpecificData(QuaZip& zip)
{
    if ( false == loadStyles(zip) ||
         false == loadSharedStrings(zip) )
	{
        error_ =
            QObject::tr("File ") + zip.getZipName() + QObject::tr(" is damaged.");
		return false;
	}

	return true;
}
