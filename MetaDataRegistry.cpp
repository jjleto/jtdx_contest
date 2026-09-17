#include "MetaDataRegistry.hpp"

#include <QMetaType>
#include <QItemEditorFactory>
#include <QStandardItemEditorCreator>

#include "Radio.hpp"
#include "FrequencyList.hpp"
#include "AudioDevice.hpp"
#include "Configuration.hpp"
#include "StationList.hpp"
#include "Transceiver.hpp"
#include "TransceiverFactory.hpp"
#include "WFPalette.hpp"

#include "FrequencyLineEdit.hpp"
#include "FrequencyDeltaLineEdit.hpp"
#include "IARURegions.hpp"

QItemEditorFactory * item_editor_factory ()
{
  static QItemEditorFactory * our_item_editor_factory = new QItemEditorFactory;
  return our_item_editor_factory;
}

void register_types ()
{
  // Radio namespace
  auto frequency_type_id = qRegisterMetaType<Radio::Frequency> ("Frequency");
  qRegisterMetaType<Radio::Frequencies> ("Frequencies");

  // This is required to preserve v1.5 "frequencies" setting for
  // backwards compatibility, without it the setting gets trashed by
  // later versions.
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<Radio::Frequencies> ("Frequencies"); */
 item_editor_factory ()->registerEditor (frequency_type_id, new QStandardItemEditorCreator<FrequencyLineEdit> ());
  auto frequency_delta_type_id = qRegisterMetaType<Radio::FrequencyDelta> ("FrequencyDelta");
  item_editor_factory ()->registerEditor (frequency_delta_type_id, new QStandardItemEditorCreator<FrequencyDeltaLineEdit> ());

  // Frequency list model
  qRegisterMetaType<FrequencyList_v2::Item> ("Item_v2");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<FrequencyList_v2::Item> ("Item_v2"); */
  qRegisterMetaType<FrequencyList_v2::FrequencyItems> ("FrequencyItems_v2");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<FrequencyList_v2::FrequencyItems> ("FrequencyItems_v2"); */
  // defunct old versions
  qRegisterMetaType<FrequencyList::Item> ("Item");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<FrequencyList::Item> ("Item"); */
  qRegisterMetaType<FrequencyList::FrequencyItems> ("FrequencyItems");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<FrequencyList::FrequencyItems> ("FrequencyItems"); */
  // Audio device
  qRegisterMetaType<AudioDevice::Channel> ("AudioDevice::Channel");

  // Configuration
  qRegisterMetaType<Configuration::DataMode> ("Configuration::DataMode");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<Configuration::DataMode> ("Configuration::DataMode"); */
  qRegisterMetaType<Configuration::Type2MsgGen> ("Configuration::Type2MsgGen");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<Configuration::Type2MsgGen> ("Configuration::Type2MsgGen"); */
  // Station details
  qRegisterMetaType<StationList::Station> ("Station");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<StationList::Station> ("Station"); */
  qRegisterMetaType<StationList::Stations> ("Stations");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<StationList::Stations> ("Stations"); */
  // Transceiver
  qRegisterMetaType<Transceiver::TransceiverState> ("Transceiver::TransceiverState");
  qRegisterMetaType<Transceiver::MODE> ("Transceiver::MODE");

  // Transceiver factory
  qRegisterMetaType<TransceiverFactory::DataBits> ("TransceiverFactory::DataBits");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::DataBits> ("TransceiverFactory::DataBits"); */
  qRegisterMetaType<TransceiverFactory::StopBits> ("TransceiverFactory::StopBits");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::StopBits> ("TransceiverFactory::StopBits"); */
  qRegisterMetaType<TransceiverFactory::Handshake> ("TransceiverFactory::Handshake");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::Handshake> ("TransceiverFactory::Handshake"); */
  qRegisterMetaType<TransceiverFactory::PTTMethod> ("TransceiverFactory::PTTMethod");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::PTTMethod> ("TransceiverFactory::PTTMethod"); */
  qRegisterMetaType<TransceiverFactory::TXAudioSource> ("TransceiverFactory::TXAudioSource");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::TXAudioSource> ("TransceiverFactory::TXAudioSource"); */
  qRegisterMetaType<TransceiverFactory::SplitMode> ("TransceiverFactory::SplitMode");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<TransceiverFactory::SplitMode> ("TransceiverFactory::SplitMode"); */
  // Waterfall palette
  // CE3TSK/qt6-port: unlike the other custom types above, WFPalette::Colours
  // had NO separate qRegisterMetaType<> () call of its own in the original
  // (Qt5) code - it relied entirely on qRegisterMetaTypeStreamOperators<> ()
  // to register the metatype (that call did both jobs in Qt5). Simply
  // commenting that call out, as done for the other types, therefore left
  // this type completely unregistered at runtime (merely Q_DECLARE_METATYPE'd
  // at compile time), which crashed (SIGBUS) as soon as an existing
  // QSettings entry for the waterfall palette was loaded on start up. Adding
  // the explicit qRegisterMetaType<> () call back restores correct runtime
  // registration; only the old QDataStream-based settings compatibility is
  // lost (palette resets to default), exactly like the other types above.
  qRegisterMetaType<WFPalette::Colours> ("Colours");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<WFPalette::Colours> ("Colours"); */
 // IARURegions
  // CE3TSK/qt6-port: same issue and same fix as WFPalette::Colours above -
  // IARURegions::Region also relied solely on qRegisterMetaTypeStreamOperators
  // for its runtime registration and crashed (SIGBUS) on start up once an
  // existing "Region" QSettings entry was loaded.
  qRegisterMetaType<IARURegions::Region> ("IARURegions::Region");
  /* qRegisterMetaTypeStreamOperators removed for Qt6: qRegisterMetaTypeStreamOperators<IARURegions::Region> ("IARURegions::Region"); */
}
