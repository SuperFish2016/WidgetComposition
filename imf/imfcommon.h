/* Copyright(C) 2016 Björn Stresing, Denis Manthey, Wolfgang Ruppel, Krispin Weiss
 *
 * This program is free software : you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QPair>
#include <QVector>


class EditRate {

public:
	// frame rates
	static const EditRate EditRate23_98;
	static const EditRate EditRate24;
	static const EditRate EditRate25;
	static const EditRate EditRate29_97;
	static const EditRate EditRate30;
	static const EditRate EditRate48;
	static const EditRate EditRate50;
	static const EditRate EditRate59_94;
	static const EditRate EditRate60;
	static const EditRate EditRate96;
	static const EditRate EditRate100;
	static const EditRate EditRate119_88;
	static const EditRate EditRate120;
	// audio sampling rates
	static const EditRate EditRate48000;
	static const EditRate EditRate96000;

	EditRate(qint32 n, qint32 d);
	EditRate() : mNumerator(0), mDenominator(0), mName() {}
	qreal GetQuotient() const { return (qreal)mNumerator / (qreal)mDenominator; }
	qint32 GetRoundendQuotient() const { return (qint32)(GetQuotient() + .5); }
	qreal GetEditUnit() const { return (qreal)mDenominator / (qreal)mNumerator; }
	qint32 GetNumerator() const { return mNumerator; }
	qint32 GetDenominator() const { return mDenominator; }
	QString GetName() const { return mName; }
	QString GetRoundedName() const { return QString::number(GetRoundendQuotient()); }
	bool IsValid() const { return (mNumerator > 0 && mDenominator > 0); }
	bool operator==(const EditRate& rhs) const;
	bool operator!=(const EditRate& rhs) const;
	bool operator<(const EditRate& rhs) const;
	bool operator>(const EditRate& rhs) const;
	EditRate& operator=(const EditRate& other) {
		mNumerator = other.GetNumerator();
		mDenominator = other.GetDenominator();
		mName = other.GetName();
	    return *this;
	}

	//! Convenience function: Returns a well known edit rate.
	static EditRate GetEditRate(const QString &rEditRateName);
	//! Convenience function: Returns the names of all well known frame rates.
	static QStringList GetFrameRateNames();

private:
	EditRate(qint32 n, qint32 d, const QString &rName) : mNumerator(n), mDenominator(d), mName(rName) {

		EditRate::mMap.push_back(this);
	}

	qint32									mNumerator;
	qint32									mDenominator;
	QString									mName;
	static QList<EditRate*>	mMap;
};

//! Represents the number of frames or samples between two time codes (e.g.: 00:00:00:00 - 00:00:00:30 ---> Dur.: 30 frames).
class Duration {

public:
	//! Generates Null duration.
	Duration() : mSamplesFrames(0) {}
	Duration(qint64 samplesOrFrames) : mSamplesFrames(samplesOrFrames) {}
	~Duration() {}
	bool IsValid() const { return (mSamplesFrames >= 0 ? true : false); }
	bool IsNull() const { return (mSamplesFrames == 0 ? true : false); }
	QString GetAsString(const EditRate &rEditRate) const;
	qint64 GetCount() const { return mSamplesFrames; }
	void SetCount(qint64 samplesOrFrames) { mSamplesFrames = samplesOrFrames; }
	bool operator==(const Duration &rOther) const { return mSamplesFrames == rOther.mSamplesFrames; }
	bool operator!=(const Duration &rOther) const { return mSamplesFrames != rOther.mSamplesFrames; }
	bool operator<(const Duration &rOther) const { return mSamplesFrames < rOther.mSamplesFrames; }
	bool operator>(const Duration &rOther) const { return mSamplesFrames > rOther.mSamplesFrames; }
	bool operator<=(const Duration &rOther) const { return mSamplesFrames <= rOther.mSamplesFrames; }
	bool operator>=(const Duration &rOther) const { return mSamplesFrames >= rOther.mSamplesFrames; }
	Duration operator+(const Duration &rOther) const { return Duration(mSamplesFrames + rOther.mSamplesFrames); }
	Duration operator-(const Duration &rOther) const { return Duration(mSamplesFrames - rOther.mSamplesFrames); }
	Duration& operator+=(const Duration &rOther) { mSamplesFrames += rOther.mSamplesFrames; return *this; }
	Duration& operator-=(const Duration &rOther) { mSamplesFrames -= rOther.mSamplesFrames; return *this; }

private:
	qint64 mSamplesFrames;
};

Duration operator*(int i, const Duration &rOther);
Duration operator*(const Duration &rOther, int i);


//! Negative time codes exist and can be calculated with but can not be printed (Timecode::GetAsString()). A negative time code is printed as time code -00:00:00:00.
class Timecode {

public:
	Timecode(const EditRate &rEditRate, qint64 hours, qint64 minutes, qint64 seconds, qint64 frames);
	Timecode(const EditRate &rEditRate, qint64 frames);
	Timecode();
	//! Duration is the number of frames between 00:00:00:00 and the constructed time code.
	explicit Timecode(const EditRate &rEditRate, Duration duration);
	~Timecode() {}
	bool IsValid() const { return (mFramesCount >= 0 && mEditRate.IsValid() ? true : false); }
	bool IsNull() const { return (mFramesCount == 0 ? true : false); }
	//! A negative time code is returned as time code -00:00:00:00.
	QString GetAsString() const;
	//WR
	QString GetFramesAsString() const;
	//! Use place marker %1, %2, %3, %4. %1 = hours, %2 = minutes, %3 = seconds, %5 = frames. A negative time code is returned as time code -00:00:00:00.
	QString GetAsString(const QString &rMarker) const;
	qint64 GetHours() const { return (mFramesCount >= 0 && mEditRate.IsValid() ? mFramesCount / (3600 * mEditRate.GetRoundendQuotient()) % 60 : 0); }
	qint64 GetMinutes() const { return (mFramesCount >= 0 && mEditRate.IsValid() ? mFramesCount / (60 * mEditRate.GetRoundendQuotient()) % 60 : 0); }
	qint64 GetSeconds() const { return (mFramesCount >= 0 && mEditRate.IsValid() ? mFramesCount / mEditRate.GetRoundendQuotient() % 60 : 0); }
	float GetSecondsF() const { return (mFramesCount / mEditRate.GetQuotient()); } // (k)
	qint64 GetFrames() const { return (mFramesCount >= 0 && mEditRate.IsValid() ? mFramesCount % mEditRate.GetRoundendQuotient() : 0); }
	qint64 GetOverallFrames() const { return mFramesCount; }
	//! The frame this time code points at. 
	qint64 GetTargetFrame() const;
	//! The positive duration between 00:00:00:00 and *this.
	Duration AsPositiveDuration() const;
	//! The positive duration between rOther and *this.
	Duration AsPositiveDuration(const Timecode &rOther) const;
	//! The duration between 00:00:00:00 and *this.
	Duration AsDuration() const;
	//! The duration between rOther and *this.
	Duration AsDuration(const Timecode &rOther) const;

	const EditRate& GetEditRate() const;
	bool operator==(const Timecode &rOther) const { return mFramesCount == rOther.mFramesCount; }
	bool operator!=(const Timecode &rOther) const { return mFramesCount != rOther.mFramesCount; }
	bool operator<(const Timecode &rOther) const { return mFramesCount < rOther.mFramesCount; }
	bool operator>(const Timecode &rOther) const { return mFramesCount > rOther.mFramesCount; }
	bool operator<=(const Timecode &rOther) const { return mFramesCount <= rOther.mFramesCount; }
	bool operator>=(const Timecode &rOther) const { return mFramesCount >= rOther.mFramesCount; }
	Timecode operator+(const Timecode &rRight);
	Timecode operator+(const Duration &rRight);
	Timecode operator-(const Timecode &rRight);
	Timecode operator-(const Duration &rRight);
	Timecode& operator++(); // prefix
	Timecode operator++(int unused); // postfix

private:
	EditRate mEditRate;
	qint64 mFramesCount;
};

Q_DECLARE_METATYPE(EditRate)
Q_DECLARE_METATYPE(Timecode)
Q_DECLARE_METATYPE(Duration)
