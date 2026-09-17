#include "soundout.h"

#include <QAudioDevice>
#include <QAudioSink>
#include <QSysInfo>
#include <qmath.h>
#include <QDebug>

#include "moc_soundout.cpp"

bool SoundOutput::checkStream () const
{
  bool result {false};
  Q_ASSERT_X (m_stream, "SoundOutput", "programming error");
  if (m_stream) {
    switch (m_stream->error ()) {
      case QAudio::OpenError: Q_EMIT error (tr ("An error opening the audio output device has occurred.")); break;
      case QAudio::IOError: Q_EMIT error (tr ("An error occurred during write to the audio output device.")); break;
      case QAudio::UnderrunError: Q_EMIT error (tr ("Audio data not being fed to the audio output device fast enough.")); break;
      case QAudio::FatalError: Q_EMIT error (tr ("Non-recoverable error, audio output device not usable at this time.")); break;
      case QAudio::NoError: result = true; break;
    }
  }
  return result;
}

// setFormat () no longer opens the stream directly: in Qt6, QAudioSink
// must be (re)constructed against a concrete, currently-valid QAudioDevice
// and format, so construction is deferred to restart () (mirrors the
// wsjt-z qt6-port approach).
void SoundOutput::setFormat (QAudioDevice const& device, unsigned channels, int frames_buffered)
{
  Q_ASSERT (0 < channels && channels < 3);
  m_device = device;
  m_channels = channels;
  m_framesBuffered = frames_buffered;
}

void SoundOutput::restart (QIODevice * source)
{
  if (!m_device.isNull ())
    {
      QAudioFormat format (m_device.preferredFormat ());
//      qDebug () << "Preferred audio output format:" << format;
      format.setChannelCount (m_channels);
      format.setSampleRate (48000);
      format.setSampleFormat (QAudioFormat::Int16);
      if (!format.isValid ())
        {
          Q_EMIT error (tr ("Requested output audio format is not valid."));
        }
      else if (!m_device.isFormatSupported (format))
        {
          Q_EMIT error (tr ("Requested output audio format is not supported on device."));
        }
      else
        {
//          qDebug () << "Selected audio output format:" << format;
          m_stream.reset (new QAudioSink (m_device, format));
          checkStream ();
          m_stream->setVolume (m_volume);
          error_ = false;

          connect (m_stream.data(), &QAudioSink::stateChanged, this, &SoundOutput::handleStateChanged);
        }
    }
  if (!m_stream)
    {
      if (!error_)
        {
          error_ = true;        // only signal error once
          Q_EMIT error (tr ("No audio output device configured."));
        }
      return;
    }
  else
    {
      error_ = false;
    }

  // This buffer size is critical for proper sound streaming. If it is
  // too short; high activity levels on the machine can starve the audio
  // buffer. On the other hand the Windows implementation seems to take
  // the length of the buffer in time to stop the audio stream even if
  // reset() is used.
  //
  // we have to set this before every start on the stream because the
  // Windows implementation seems to forget the buffer size after a stop.
  if (m_framesBuffered > 0)
    {
      m_stream->setBufferSize (m_stream->format ().bytesForFrames (m_framesBuffered));
    }
  m_stream->start (source);
}

void SoundOutput::suspend ()
{ if (m_stream && QAudio::ActiveState == m_stream->state ()) { m_stream->suspend (); checkStream (); } }

void SoundOutput::resume ()
{ if (m_stream && QAudio::SuspendedState == m_stream->state ()) { m_stream->resume (); checkStream (); } }

void SoundOutput::reset ()
{ if (m_stream) { m_stream->reset (); checkStream (); } }

void SoundOutput::stop ()
{ if (m_stream) { m_stream->reset (); m_stream->stop (); } }

qreal SoundOutput::attenuation () const
{ return -(20. * qLn (m_volume) / qLn (10.)); }

void SoundOutput::setAttenuation (qreal a)
{
  Q_ASSERT (0.0 <= a && a <= 450.1);
  m_volume = qPow(10.0, -a/20.0)*0.9; // 0.9 is the workaround to prevent audio stream distortion in VAC software
  //  qDebug () << "SoundOut: attn = " << a << ", vol = " << m_volume;
  if (m_stream) m_stream->setVolume (m_volume);
}

void SoundOutput::resetAttenuation ()
{ m_volume = 1.; if (m_stream) m_stream->setVolume (m_volume); }

void SoundOutput::handleStateChanged (QAudio::State newState)
{
  // qDebug () << "SoundOutput::handleStateChanged: newState:" << newState;
  switch (newState) {
    case QAudio::IdleState: Q_EMIT status (tr ("Idle")); break;
    case QAudio::ActiveState: Q_EMIT status (tr ("Sending")); break;
    case QAudio::SuspendedState: Q_EMIT status (tr ("Suspended")); break;
#if QT_VERSION >= QT_VERSION_CHECK (5, 10, 0) && QT_VERSION < QT_VERSION_CHECK (6, 0, 0)
    case QAudio::InterruptedState: Q_EMIT status (tr ("Interrupted")); break;
#endif
    case QAudio::StoppedState: if (!checkStream ()) Q_EMIT status (tr ("Error")); else Q_EMIT status (tr ("Stopped")); break;
  }
}
