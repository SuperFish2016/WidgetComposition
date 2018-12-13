#ifndef COMMANDS_H
#define COMMANDS_H
#include "commands.h"
#include "imf/imfcommon.h"
#include <QUndoCommand>
#include <QPointF>
class GraphicsWidgetSequence;
class GraphicsWidgetSegment;
class GraphicsWidgetMarkerIndicator;
class AbstractWidgetTrackDetails;
class WidgetComposition;
class AbstractGraphicsWidgetResources;


class SetEntryPointCommand : public QUndoCommand {

public:
    SetEntryPointCommand(AbstractGraphicsWidgetResources *pResource, const Duration &rOldEntryPoint,
                         const Duration &rNewEntryPoint, QUndoCommand *pParent = NULL);
    virtual ~SetEntryPointCommand() {}
    virtual void undo();
    //! Called once when pushed on Undo Stack.
    virtual void redo();

private:
    Q_DISABLE_COPY(SetEntryPointCommand)
    AbstractGraphicsWidgetResources *mpResource;
    Duration mOldEntryPoint;
    Duration mNewEntryPoint;
};


class SetSourceDurationCommand : public QUndoCommand {

public:
    SetSourceDurationCommand(AbstractGraphicsWidgetResources *pResource, const Duration &rOldSourceDuration,
                             const Duration &rNewSourceDuration, QUndoCommand *pParent = NULL);
    virtual ~SetSourceDurationCommand() {}
    virtual void undo();
    //! Called once when pushed on Undo Stack.
    virtual void redo();

private:
    Q_DISABLE_COPY(SetSourceDurationCommand)
    AbstractGraphicsWidgetResources *mpResource;
    Duration mOldSourceDuration;
    Duration mNewSourceDuration;
};

class AddSequenceCommand : public QUndoCommand
{
public:
    AddSequenceCommand(GraphicsWidgetSequence* sequence, int index, GraphicsWidgetSegment* segment, QUndoCommand *parent = NULL);
    ~AddSequenceCommand(){}
    virtual void undo();
    virtual void redo();
private:
    Q_DISABLE_COPY(AddSequenceCommand)

    GraphicsWidgetSegment* segment_;
    GraphicsWidgetSequence* sequence_;
    int  old_sequence_index_;
    bool is_redone;
};

class AddTrackDetailsCommand : public QUndoCommand {

public:
    AddTrackDetailsCommand(AbstractWidgetTrackDetails *track_details, WidgetComposition *widget_compostion, int trackIndex, QUndoCommand *parent = NULL);
    virtual ~AddTrackDetailsCommand();
    virtual void undo();
    //! Called once when pushed on Undo Stack.
    virtual void redo();

private:
    Q_DISABLE_COPY(AddTrackDetailsCommand)
    AbstractWidgetTrackDetails* track_details_;
    WidgetComposition*  widget_composition_;
    int  old_track_index_;
    bool is_redone;
};

class AddResourceCommand : public QUndoCommand
{
public:
    AddResourceCommand(GraphicsWidgetSequence* sequence, AbstractGraphicsWidgetResources* resource, int index, QUndoCommand* parent);
    virtual ~AddResourceCommand();
    virtual void undo();
    virtual void redo();
private:
    Q_DISABLE_COPY(AddResourceCommand)
    AbstractGraphicsWidgetResources* resource_;
    GraphicsWidgetSequence*  sequence_;
    qint32 old_resource_index_;
    bool   is_redone;
};

class DeleteMarkerCommand : public QUndoCommand
{
public:
    DeleteMarkerCommand(GraphicsWidgetMarkerIndicator* marker, QUndoCommand* parent = NULL);
    ~DeleteMarkerCommand();
    virtual void undo();
    virtual void redo();
private:
    Q_DISABLE_COPY(DeleteMarkerCommand)

    GraphicsWidgetMarkerIndicator *marker_;
    QPointF pos_;
    bool    is_redone;
};

#endif // COMMANDS_H
