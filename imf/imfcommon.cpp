#include <QDebug>
#include "imfcommon.h"
#include <QStringList>


Timecode::Timecode(const EditRate &rEditRate, qint64 hours, qint64 minutes, qint64 seconds, qint64 frames) :
mEditRate(rEditRate), mFramesCount((seconds + minutes * 60 + hours * 3600) * mEditRate.GetRoundendQuotient() + frames) {

}

Timecode::Timecode(const EditRate &rEditRate, Duration duration /*= 0*/) :
mEditRate(rEditRate), mFramesCount(duration.GetCount()) {

}

Timecode::Timecode(const EditRate &rEditRate, qint64 frames) :
mEditRate(rEditRate), mFramesCount(frames) {

}

Timecode::Timecode() :
mEditRate(EditRate()), mFramesCount(0) {

}

Timecode Timecode::operator+(const Timecode &rRight) {

	if(mEditRate != rRight.mEditRate) {
		qCritical() << "Timcode values with different Edit Rates detected.";
		return *this;
	}
	Timecode result(*this);
	result.mFramesCount += rRight.mFramesCount;
	return result;
}

Timecode Timecode::operator+(const Duration &rRight) {

	Timecode result(*this);
	result.mFramesCount += rRight.GetCount();
	return result;
}

Timecode Timecode::operator-(const Timecode &rRight) {

	if(mEditRate != rRight.mEditRate) {
		qCritical() << "Timcode values with different Edit Rates detected.";
		return *this;
	}
	Timecode result(*this);
	result.mFramesCount -= rRight.mFramesCount;
	return result;
}

Timecode Timecode::operator-(const Duration &rRight) {

	Timecode result(*this);
	result.mFramesCount -= rRight.GetCount();
	return result;
}

Duration Timecode::AsPositiveDuration() const {

	return std::abs(mFramesCount);
}

Duration Timecode::AsPositiveDuration(const Timecode &rOther) const {

	return std::abs(mFramesCount - rOther.mFramesCount);
}

Timecode& Timecode::operator++() {

	mFramesCount += 1;
	return *this;
}

Timecode Timecode::operator++(int unused) {

	Timecode result = *this;
	++(*this);
	return result;
}

QString Timecode::GetAsString() const {

	QString ret;
	if(mFramesCount < 0) ret.append("-");
	return ret.append(QString("%1:%2:%3:%4").arg(GetHours(), 2, 10, QChar('0')).arg(GetMinutes(), 2, 10, QChar('0')).arg(GetSeconds(), 2, 10, QChar('0')).arg(GetFrames(), 2, 10, QChar('0')));
}

QString Timecode::GetFramesAsString() const {

	QString ret;
	if(mFramesCount < 0) ret.append("-");
    return ret.append(QString("%1").arg((mFramesCount < 0 ? 0 : mFramesCount)));
}


QString Timecode::GetAsString(const QString &rMarker) const {

	// TODO: improve
	QString ret;
	QStringList list = rMarker.split("%", QString::SkipEmptyParts);
	if(mFramesCount < 0) ret.append("-");
	for(int i = 0; i < list.size(); i++) {
		if(list.at(i).startsWith(QChar('1'))) ret.append(QString("%").append(list.at(i)).arg(GetHours(), 2, 10, QChar('0')));
		else if(list.at(i).startsWith(QChar('2'))) ret.append(QString("%").append(list.at(i)).arg(GetMinutes(), 2, 10, QChar('0')));
		else if(list.at(i).startsWith(QChar('3'))) ret.append(QString("%").append(list.at(i)).arg(GetSeconds(), 2, 10, QChar('0')));
		else if(list.at(i).startsWith(QChar('4'))) ret.append(QString("%").append(list.at(i)).arg(GetFrames(), 2, 10, QChar('0')));
	}
	return ret;
}

const EditRate& Timecode::GetEditRate() const {

	return mEditRate;
}

qint64 Timecode::GetTargetFrame() const {

	qint64 target_frame = mFramesCount;
	if (mFramesCount < 0) target_frame = 0;
	return target_frame;
}

Duration Timecode::AsDuration() const {

	return mFramesCount;
}

Duration Timecode::AsDuration(const Timecode &rOther) const {

	return (mFramesCount - rOther.mFramesCount);
}

QList<EditRate*> EditRate::mMap;
const EditRate EditRate::EditRate23_98(24000, 1001, "23.976");
const EditRate EditRate::EditRate24(24, 1, "24");
const EditRate EditRate::EditRate25(25, 1, "25");
const EditRate EditRate::EditRate29_97(30000, 1001, "29.97");
const EditRate EditRate::EditRate30(30, 1, "30");
const EditRate EditRate::EditRate48(48, 1, "48");
const EditRate EditRate::EditRate50(50, 1, "50");
const EditRate EditRate::EditRate59_94(60000, 1001, "59.94");
const EditRate EditRate::EditRate60(60, 1, "60");
const EditRate EditRate::EditRate96(96, 1, "96");
const EditRate EditRate::EditRate100(100, 1, "100");
const EditRate EditRate::EditRate119_88(120000, 1001, "119.88");
const EditRate EditRate::EditRate120(120, 1, "120");

const EditRate EditRate::EditRate48000(48000, 1, "48000");
const EditRate EditRate::EditRate96000(96000, 1, "96000");

EditRate::EditRate(qint32 n, qint32 d) : mName() {

	mNumerator = (d == 1000) ? n/1000 : n;
	mDenominator = (d == 1000) ? 1 : d;

	for(int i = 0; i < EditRate::mMap.size(); i++) {
		EditRate *p_edit_rate = EditRate::mMap.at(i);
		if(p_edit_rate && p_edit_rate->mNumerator == mNumerator && p_edit_rate->mDenominator == mDenominator) mName = p_edit_rate->mName;
	}
}

bool EditRate::operator==(const EditRate& rhs) const {

	return (rhs.mNumerator == mNumerator && rhs.mDenominator == mDenominator);
}

bool EditRate::operator!=(const EditRate& rhs) const {

	return (rhs.mNumerator != mNumerator || rhs.mDenominator != mDenominator);
}

bool EditRate::operator<(const EditRate& rhs) const {

	if(mNumerator < rhs.mNumerator) return true;
	if(mNumerator == rhs.mNumerator && mDenominator < rhs.mDenominator) return true;
	return false;
}

bool EditRate::operator>(const EditRate& rhs) const {

	if(mNumerator > rhs.mNumerator) return true;
	if(mNumerator == rhs.mNumerator && mDenominator > rhs.mDenominator) return true;
	return false;
}

EditRate EditRate::GetEditRate(const QString &rEditRateName) {

	EditRate ret;
	for(int i = 0; i < EditRate::mMap.size(); i++) {
		if(EditRate::mMap.at(i) && rEditRateName.compare(EditRate::mMap.at(i)->GetName(), Qt::CaseSensitive) == 0) ret = *EditRate::mMap.at(i);
	}
	return ret;
}

QStringList EditRate::GetFrameRateNames() {

	QStringList ret;
	for(int i = 0; i < EditRate::mMap.size(); i++) {
		if(EditRate::mMap.at(i) && *EditRate::mMap.at(i) != EditRate48000 && *EditRate::mMap.at(i) != EditRate96000) ret.push_back(EditRate::mMap.at(i)->GetName());
	}
	return ret;
}

QString Duration::GetAsString(const EditRate &rEditRate) const {

	return Timecode(rEditRate, 0, 0, 0, mSamplesFrames).GetAsString();
}

Duration operator*(int i, const Duration &rOther) {

	return Duration(rOther.GetCount() * i);
}

Duration operator*(const Duration &rOther, int i) {

	return Duration(rOther.GetCount() * i);
}
