/***************************************************************************
                              qgsjoindialog.cpp
                              --------------------
  begin                : July 10, 2010
  copyright            : (C) 2010 by Marco Hugentobler
  email                : marco dot hugentobler at sourcepole dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsjoindialog.h"
#include "qgsmaplayer.h"
#include "qgsproject.h"
#include "qgsvectordataprovider.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerjoininfo.h"
#include "qgsmaplayercombobox.h"
#include "qgsfieldcombobox.h"

#include <QStandardItemModel>
#include <QPushButton>

QgsJoinDialog::QgsJoinDialog( QgsVectorLayer *layer, QList<QgsMapLayer *> alreadyJoinedLayers, QWidget *parent, Qt::WindowFlags f )
  : QDialog( parent, f )
  , mLayer( layer )
{
  setupUi( this );

  if ( !mLayer )
  {
    return;
  }
  // adds self layer to the joined layer (cannot join to itself)
  alreadyJoinedLayers.append( layer );

  mTargetFieldComboBox->setLayer( mLayer );

  mJoinLayerComboBox->setFilters( QgsMapLayerProxyModel::VectorLayer );
  mJoinLayerComboBox->setExceptedLayerList( alreadyJoinedLayers );
  connect( mJoinLayerComboBox, &QgsMapLayerComboBox::layerChanged, mJoinFieldComboBox, &QgsFieldComboBox::setLayer );
  connect( mJoinLayerComboBox, &QgsMapLayerComboBox::layerChanged, this, &QgsJoinDialog::joinedLayerChanged );

  mCacheInMemoryCheckBox->setChecked( true );

  QgsMapLayer *joinLayer = mJoinLayerComboBox->currentLayer();
  if ( joinLayer && joinLayer->isValid() )
  {
    mJoinFieldComboBox->setLayer( joinLayer );
    joinedLayerChanged( joinLayer );
  }

  connect( mJoinLayerComboBox, &QgsMapLayerComboBox::layerChanged, this, &QgsJoinDialog::checkDefinitionValid );
  connect( mJoinFieldComboBox, &QgsFieldComboBox::fieldChanged, this, &QgsJoinDialog::checkDefinitionValid );
  connect( mTargetFieldComboBox, &QgsFieldComboBox::fieldChanged, this, &QgsJoinDialog::checkDefinitionValid );

  checkDefinitionValid();
}

QgsJoinDialog::~QgsJoinDialog()
{
}

void QgsJoinDialog::setJoinInfo( const QgsVectorLayerJoinInfo &joinInfo )
{
  mJoinLayerComboBox->setLayer( joinInfo.joinLayer() );
  mJoinFieldComboBox->setField( joinInfo.joinFieldName() );
  mTargetFieldComboBox->setField( joinInfo.targetFieldName() );
  mCacheInMemoryCheckBox->setChecked( joinInfo.isUsingMemoryCache() );
  if ( joinInfo.prefix().isNull() )
  {
    mUseCustomPrefix->setChecked( false );
  }
  else
  {
    mUseCustomPrefix->setChecked( true );
    mCustomPrefix->setText( joinInfo.prefix() );
  }

  QStringList *lst = joinInfo.joinFieldNamesSubset();
  mUseJoinFieldsSubset->setChecked( lst && !lst->isEmpty() );
  QAbstractItemModel *model = mJoinFieldsSubsetView->model();
  if ( model )
  {
    for ( int i = 0; i < model->rowCount(); ++i )
    {
      QModelIndex index = model->index( i, 0 );
      if ( lst && lst->contains( model->data( index, Qt::DisplayRole ).toString() ) )
      {
        model->setData( index, Qt::Checked, Qt::CheckStateRole );
      }
      else
      {
        model->setData( index, Qt::Unchecked, Qt::CheckStateRole );
      }
    }
  }
}

QgsVectorLayerJoinInfo QgsJoinDialog::joinInfo() const
{
  QgsVectorLayerJoinInfo info;
  info.setJoinLayer( qobject_cast<QgsVectorLayer *>( mJoinLayerComboBox->currentLayer() ) );
  info.setJoinFieldName( mJoinFieldComboBox->currentField() );
  info.setTargetFieldName( mTargetFieldComboBox->currentField() );
  info.setUsingMemoryCache( mCacheInMemoryCheckBox->isChecked() );

  if ( mUseCustomPrefix->isChecked() )
    info.setPrefix( mCustomPrefix->text() );
  else
    info.setPrefix( QString() );

  if ( mUseJoinFieldsSubset->isChecked() )
  {
    QStringList lst;
    QAbstractItemModel *model = mJoinFieldsSubsetView->model();
    if ( model )
    {
      for ( int i = 0; i < model->rowCount(); ++i )
      {
        QModelIndex index = model->index( i, 0 );
        if ( model->data( index, Qt::CheckStateRole ).toInt() == Qt::Checked )
          lst << model->data( index ).toString();
      }
    }
    info.setJoinFieldNamesSubset( new QStringList( lst ) );
  }

  return info;
}

bool QgsJoinDialog::createAttributeIndex() const
{
  return mCreateIndexCheckBox->isChecked();
}

void QgsJoinDialog::joinedLayerChanged( QgsMapLayer *layer )
{
  mJoinFieldComboBox->clear();

  QgsVectorLayer *vLayer = dynamic_cast<QgsVectorLayer *>( layer );
  if ( !vLayer )
  {
    return;
  }

  mUseJoinFieldsSubset->setChecked( false );
  QStandardItemModel *subsetModel = new QStandardItemModel( this );
  Q_FOREACH ( const QgsField &field, vLayer->fields() )
  {
    QStandardItem *subsetItem = new QStandardItem( field.name() );
    subsetItem->setCheckable( true );
    //subsetItem->setFlags( subsetItem->flags() | Qt::ItemIsUserCheckable );
    subsetModel->appendRow( subsetItem );
  }
  mJoinFieldsSubsetView->setModel( subsetModel );

  QgsVectorDataProvider *dp = vLayer->dataProvider();
  bool canCreateAttrIndex = dp && ( dp->capabilities() & QgsVectorDataProvider::CreateAttributeIndex );
  if ( canCreateAttrIndex )
  {
    mCreateIndexCheckBox->setEnabled( true );
  }
  else
  {
    mCreateIndexCheckBox->setEnabled( false );
    mCreateIndexCheckBox->setChecked( false );
  }

  if ( !mUseCustomPrefix->isChecked() )
  {
    mCustomPrefix->setText( layer->name() + '_' );
  }
}

void QgsJoinDialog::checkDefinitionValid()
{
  buttonBox->button( QDialogButtonBox::Ok )->setEnabled( mJoinLayerComboBox->currentIndex() != -1
      && mJoinFieldComboBox->currentIndex() != -1
      && mTargetFieldComboBox->currentIndex() != -1 );
}
