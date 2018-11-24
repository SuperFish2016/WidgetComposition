#include "commands.h"
#include "graphics/graphicswidgetsegment.h"
#include "graphics/graphicswidgetsequence.h"
#include "graphics/graphicswidgetresources.h"
#include "widget/widgettrackdetails.h"
#include "widget/widgetcomposition.h"
AddSequenceCommand::AddSequenceCommand(GraphicsWidgetSequence *sequence, int index, GraphicsWidgetSegment *segment, QUndoCommand* parent)
    :QUndoCommand(parent), sequence_(sequence), segment_(segment), old_sequence_index_(index)
{

}

void AddSequenceCommand::undo()
{
    if(old_sequence_index_ >= 0)
    {
        sequence_->hide();
        segment_->deleteSequence(sequence_);
        sequence_->setParentItem(nullptr);
        ;
    }
}

void AddSequenceCommand::redo()
{
    if(old_sequence_index_ >= 0)
    {
        segment_->addSequence(sequence_, old_sequence_index_);
        sequence_->show();

    }
}

AddTrackDetailsCommand::AddTrackDetailsCommand(AbstractWidgetTrackDetails *track_details, WidgetComposition *widget_composition, int trackIndex, QUndoCommand *parent)
    :QUndoCommand(parent), track_details_(track_details), widget_composition_(widget_composition), old_track_index_(trackIndex), is_redone(false)
{

}

AddTrackDetailsCommand::~AddTrackDetailsCommand() {

    if(is_redone == false && track_details_) track_details_->deleteLater(); // TODO: Is this safe.
}

void AddTrackDetailsCommand::undo() {

    if(old_track_index_ >= 0) {
        track_details_->hide();
        widget_composition_->delete_track_details(track_details_);
        is_redone = false;
    }
}

void AddTrackDetailsCommand::redo() {

    if(old_track_index_ >= 0) {
        widget_composition_->add_track_details(track_details_, old_track_index_);
        track_details_->show();
        is_redone = true;
    }
}

AddResourceCommand::AddResourceCommand(GraphicsWidgetSequence *sequence, AbstractGraphicsWidgetResources *resource, int index, QUndoCommand *parent)
    :QUndoCommand(parent), sequence_(sequence), resource_(resource), old_resource_index_(index), is_redone(false)
{

}

AddResourceCommand::~AddResourceCommand()
{
    if(is_redone == false && resource_)
        resource_->deleteLater();
}

void AddResourceCommand::undo()
{
    if(old_resource_index_ >= 0) {
        sequence_->deleteResource(resource_);
        resource_->hide();
        resource_->setParentItem(nullptr); // Gets top level item. Deleted by scene.
        is_redone = false;
        sequence_->layout()->activate();
    }
}

void AddResourceCommand::redo()
{
    if(old_resource_index_ >= 0) {
        sequence_->addResource(resource_, old_resource_index_);
        resource_->show();
        is_redone = true;
        sequence_->layout()->activate();
    }
}

DeleteMarkerCommand::DeleteMarkerCommand(GraphicsWidgetMarkerIndicator *marker, QUndoCommand *parent)
    :marker_(marker)
{

}

DeleteMarkerCommand::~DeleteMarkerCommand()
{
    if(is_redone && marker_)
    {
        marker_->deleteLater();
    }
}

void DeleteMarkerCommand::redo()
{
    pos_ = marker_->pos();
    marker_->hide();
    marker_->setParentItem(nullptr);
    is_redone = true;
}

void DeleteMarkerCommand::undo()
{
    marker_->setPos(pos_);
    marker_->show();
    is_redone = false;
}
