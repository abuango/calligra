/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright ( C ) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright ( C ) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRSHAPEANIMATIONS_H
#define KPRSHAPEANIMATIONS_H

#include <QList>
#include <QAbstractTableModel>
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationStep.h"

#include "stage_export.h"

class KPrDocument;

struct AnimationTmpData
{
    int group;
    KPrShapeAnimation::NodeType nodeType;
};


/**
 * Model for Animations data of each KPrPage
 */
class STAGE_EXPORT KPrShapeAnimations : public QAbstractTableModel
{
    Q_OBJECT
public:

    /// Time to be updated
    enum TimeUpdated {
        BeginTime,
        DurationTime,
        BothTimes
    };

    /// column names
    enum ColumnNames {
        Group = 0,
        StepCount = 1,
        TriggerEvent = 2,
        Name = 3,
        ShapeThumbnail = 4,
        AnimationIcon = 5,
        StartTime = 6,
        Duration = 7,
        AnimationClass = 8,
        NodeType = 9
    };

    explicit KPrShapeAnimations(QObject *parent = 0);
    ~KPrShapeAnimations();

    /// Model Methods
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
               int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                     int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) { return false; }
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole);



    void init(const QList<KPrAnimationStep *> animations);
    /**
     * Add animation to the animations
     *
     * It the animation for this shape/step already exists it is replaced by the given one
     *
     * @parama animation the animation to insert
     */
    void add( KPrShapeAnimation * animation );

    /**
     * Remove  animation to the animations
     *
     * @parama animation the animation to remove
     */
    void remove( KPrShapeAnimation * animation );

    /**
     * @brief Insert a new step on the steps list
     *
     * @param i position in which the step will be inserted
     * @param step step to be inserted
     */
    void insertStep(const int i, KPrAnimationStep *step);

    /**
     * @brief Remove a step on the steps list
     *
     * @param step step to be removed
     */
    void removeStep(KPrAnimationStep *step);

    /**
     * @brief Swap steps in positions i and j
     *
     * @param i position of the first step
     * @param j position of the second step
     */
    void swapSteps(int i, int j);

    void swapAnimations(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation);

    /**
     * @brief Replace old animation with new animation
     *
     * @param oldAnimation animation to be replaced
     * @param newAnimation
     */
    void replaceAnimation(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation);


    /**
     * Get the animations for the given step
     *
     * @param step the step for which the animations should be returned
     * @return A map of the shape -> animation if the animation is 0 the shape
     *         is not visible
     */
/*    QMap<KoShape *, KPrShapeAnimation *> animations( int step ) const;
*/
    /**
     * Get all steps
     */
    QList<KPrAnimationStep *> steps() const;

    /// Save a edit command
    void endTimeLineEdition();

    /**
     * @brief Set animation begin and duration
     *
     * @param index index of the animation
     * @param begin time in miliseconds
     * @param duration time in miliseconds
     */
    void setTimeRange(KPrShapeAnimation *item, const int begin, const int duration);

    /// requiere to send commands
    void setDocument(KPrDocument *document);

    /**
     * Get the end time for the given animation
     *
     * @param index of the animation
     * @return the time in miliseconds of the animation end
     */
    int animationEndByIndex(const QModelIndex &index);

    /**
     * Get the scale begin time for the given animation
     *
     * @param index of the animation
     * @return the time in miliseconds where the scale begin (relative to the parent onclik animation)
     */
    int scaleBeginForAnimation(const QModelIndex &index);

    /**
     * @brief Replace animation in the given index
     *
     * @param index index of the animation
     * @param newAnimation animation to be used to replace
     */
    QModelIndex replaceAnimation(const QModelIndex &index, KPrShapeAnimation *newAnimation);

    /**
     * @brief Create command to change trigger event of the animation on index
     *
     * @param index index of the animation
     * @param type new Node Type for the animation
     */
    bool setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::NodeType type);

    /**
     * @brief Change trigger event of the animation
     *
     * @param animation
     * @param type new Node Type for the animation
     */
    bool setNodeType(KPrShapeAnimation *animation, const KPrShapeAnimation::NodeType type);

    /**
     * @brief Redefine start of the animation if is moved below the minimun limit
     * of its animation trigger event scale.
     *
     * @param mIndex index of the animation
     */
    void recalculateStart(const QModelIndex &mIndex);

    /**
     * @brief Move animation up in the animation list
     * Redefine trigger event if it's necessary
     *
     * @param index of the animation
     */
    QModelIndex moveUp(const QModelIndex &index);

    /**
     * @brief Move animation down in the animation list
     * Redefine trigger event if it's necessary
     *
     * @param index of the animation
     */
    QModelIndex moveDown(const QModelIndex &index);

    /**
     * @brief Move animation from oldRow to newRow
     * Redefine trigger event if it's necessary
     *
     * @param index of the animation
     */
    QModelIndex moveAnimation(int oldRow, int newRow);

    /**
     * @brief remove animation on index
     *
     * @param index of the animation to be removed
     */
    QModelIndex removeAnimationByIndex(const QModelIndex &index);

    /**
     * @brief Return the shape of the animation on given index
     *
     * @param index of the animation
     */
    KoShape *shapeByIndex(const QModelIndex &index);

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);

    /**
     * @brief Set begin time for the animation on index
     *
     * @param index of the animation
     */
    void setBeginTime(const QModelIndex &index, const int begin);

    /**
     * @brief Set duration for the animation on index
     *
     * @param index of the animation
     */
    void setDuration(const QModelIndex &index, const int duration);

    KPrShapeAnimation *animationByRow(const int row) const;

    /**
     * @brief add new animation after index
     *
     * @param newAnimation animation to be inserted
     * @param previousAnimation index of the previous animation
     */
    void insertNewAnimation(KPrShapeAnimation *newAnimation, const QModelIndex &previousAnimation);

    QModelIndex indexByAnimation(KPrShapeAnimation *animation);

    void resyncStepsWithAnimations();

    KPrShapeAnimation::NodeType triggerEventByIndex(const QModelIndex &index);

public slots:
    /// Notify a external edition of begin or end time
    void notifyAnimationEdited();

    /// Notify an external edition of an animation
    void notifyAnimationChanged(KPrShapeAnimation *animation);

    /// Notify if an animation set as OnClick has changed of trigger event
    void notifyOnClickEventChanged();

signals:
    void timeScaleModified();
    void onClickEventChanged();

private:
    KPrShapeAnimation *animationByRow(const int row, AnimationTmpData &currentData) const;
    QString getAnimationName(KPrShapeAnimation *animation, bool omitSubType = false) const;
    QPixmap getAnimationShapeThumbnail(KPrShapeAnimation *animation) const;
    QPixmap getAnimationIcon(KPrShapeAnimation *animation) const;
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;
    void setTimeRangeIncrementalChange(KPrShapeAnimation *item, const int begin, const int duration, TimeUpdated updatedTimes);
    QList<KPrShapeAnimation *> getWithPreviousSiblings(KPrShapeAnimation *animation);
    QList<KPrAnimationSubStep *> getSubSteps(int start, int end, KPrAnimationStep *step);
    bool createTriggerEventEditCmd(KPrShapeAnimation *animation, KPrShapeAnimation::NodeType oldType, KPrShapeAnimation::NodeType newType);

    QList<KPrAnimationStep *> m_shapeAnimations;
    KPrShapeAnimation *m_currentEditedAnimation;
    bool m_firstEdition;
    int m_oldBegin;
    int m_oldDuration;
    KPrDocument *m_document;
};

#endif /* KPRSHAPEANIMATIONS_H */
