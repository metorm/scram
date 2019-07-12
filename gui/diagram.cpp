/*
 * Copyright (C) 2016-2018 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// Implementation of diagram graphics.

#include "diagram.h"

#include <cmath>

#include <unordered_map>
#include <vector>

#include <QApplication>
#include <QFontMetrics>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QStyleOptionGraphicsItem>

#include "src/ext/find_iterator.h"

#include "guiassert.h"
#include "overload.h"

namespace scram::gui::diagram {

QSizeF Event::m_size = {14, 11};
double Event::m_baseHeight = 6.5;
const double Event::m_idBoxLength = 14;
const double Event::m_labelBoxHeight = 4;

int Event::lineWidth = 2;
bool Event::drawDescription = true;

Event::Event(model::Element *event, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_event(event), m_typeGraphics(nullptr)
{
    m_labelConnection = QObject::connect(event, &model::Element::labelChanged,
                                         [this] { update(); });
    m_idConnection = QObject::connect(event, &model::Element::idChanged,
                                      [this] { update(); });
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

Event::~Event() noexcept
{
    QObject::disconnect(m_labelConnection);
    QObject::disconnect(m_idConnection);
}

QSizeF Event::units() const
{
    double h = QApplication::fontMetrics().height();
    return {h / 2, h};
}

double Event::width() const
{
    return m_size.width() * units().width();
}

void Event::setDrawDescription(bool drawDescription)
{
    Event::drawDescription = drawDescription;
    m_baseHeight = drawDescription ? 6.5 : 1.5;
    m_size = drawDescription ? QSizeF(16, 11) : QSizeF(16, 6);
}

bool Event::getDrawDescription()
{
    return drawDescription;
}

void Event::setTypeGraphics(QAbstractGraphicsShapeItem *item)
{
    delete m_typeGraphics;
    m_typeGraphics = item;
    m_typeGraphics->setParentItem(this);
}

QRectF Event::boundingRect() const
{
    double w = units().width();
    double h = units().height();
    double labelBoxWidth = m_size.width() * w;
    return QRectF(-labelBoxWidth / 2, 0, labelBoxWidth, m_baseHeight * h);
}

void Event::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget * /*widget*/)
{
    {
        QPen newPen(painter->pen());
        newPen.setWidthF(lineWidth);
        painter->setPen(newPen);

        m_typeGraphics->setPen(newPen);
        m_typeGraphics->setPos(0, m_baseHeight * units().height());
    }

    if (option->state & QStyle::State_Selected)
        painter->setBrush(QColor("cyan"));

    double w = units().width();
    double h = units().height();
    if (drawDescription) {
        double labelBoxWidth = m_size.width() * w;
        QRectF rect(-labelBoxWidth / 2, 0, labelBoxWidth, m_labelBoxHeight * h);
        painter->drawRect(rect);
        painter->drawText(
            rect, Qt::AlignCenter | Qt::TextWordWrap,
            painter->fontMetrics().elidedText(
                m_event->label(), Qt::ElideRight,
                static_cast<int>(labelBoxWidth * (m_labelBoxHeight - 0.5))));

        painter->drawLine(QPointF(0, m_labelBoxHeight * h),
                          QPointF(0, (m_labelBoxHeight + 1) * h));
    }

    double idBoxWidth = m_idBoxLength * w;
    double nameRectYStart = drawDescription ? (m_labelBoxHeight + 1) : 0;

    QRectF nameRect(-idBoxWidth / 2, nameRectYStart * h, idBoxWidth, h);
    painter->drawRect(nameRect);
    painter->drawText(
        nameRect, Qt::AlignCenter,
        painter->fontMetrics().elidedText(m_event->id(), Qt::ElideRight,
                                          static_cast<int>(idBoxWidth)));

    painter->drawLine(QPointF(0, (nameRectYStart + 1) * h),
                      QPointF(0, (nameRectYStart + 1.5) * h));
}

BasicEvent::BasicEvent(model::BasicEvent *event, QGraphicsItem *parent)
    : Event(event, parent)
{
    double d = int(m_size.height() - m_baseHeight) * units().height();
    Event::setTypeGraphics(new QGraphicsEllipseItem(-d / 2, 0, d, d));
}

HouseEvent::HouseEvent(model::HouseEvent *event, QGraphicsItem *parent)
    : Event(event, parent)
{
    double h = int(m_size.height() - m_baseHeight) * units().height();
    double y0 = h * 0.25;
    Event::setTypeGraphics(new QGraphicsPolygonItem(
        {{{0, 0}, {-h / 2, y0}, {-h / 2, h}, {h / 2, h}, {h / 2, y0}}}));
}

UndevelopedEvent::UndevelopedEvent(model::BasicEvent *event,
                                   QGraphicsItem *parent)
    : Event(event, parent)
{
    double h = int(m_size.height() - m_baseHeight) * units().height();
    double a = h / std::sqrt(2);
    auto *diamond = new QGraphicsRectItem(-a / 2, (h - a) / 2, a, a);
    diamond->setTransformOriginPoint(0, h / 2);
    diamond->setRotation(45);
    Event::setTypeGraphics(diamond);
}

TransferIn::TransferIn(model::Gate *event, QGraphicsItem *parent)
    : Event(event, parent)
{
    double d = int(m_size.height() - m_baseHeight) * units().height();
    Event::setTypeGraphics(
        new QGraphicsPolygonItem({{{0, 0}, {-d / 2, d}, {d / 2, d}}}));
}

const QSizeF Gate::m_maxSize = {6, 3};
const double Gate::m_space = 1;

Gate::Gate(model::Gate *event, model::Model *model,
           std::unordered_map<const mef::Gate *, Gate *> *transfer,
           QGraphicsItem *parent)
    : Event(event, parent)
{
    double availableHeight =
        m_size.height() - m_baseHeight - m_maxSize.height();
    double connectingLineYStart =
        (m_baseHeight + m_maxSize.height()) * units().height();
    QPainterPath connectingPath;
    connectingPath.moveTo(0, connectingLineYStart);
    connectingPath.lineTo(0, connectingLineYStart
                                 + (availableHeight - 1) * units().height());

    Event::setTypeGraphics(getGateGraphicsType(event->type()).release());
    struct
    {
        Event *operator()(const mef::BasicEvent *arg)
        {
            model::BasicEvent *proxyEvent =
                m_model->basicEvents().find(arg)->get();
            switch (proxyEvent->flavor()) {
            case model::BasicEvent::Basic:
                return new BasicEvent(proxyEvent, m_parent);
            case model::BasicEvent::Undeveloped:
                return new UndevelopedEvent(proxyEvent, m_parent);
            }
            GUI_ASSERT(false && "Unexpected event flavor", nullptr);
        }
        Event *operator()(const mef::HouseEvent *arg)
        {
            return new HouseEvent(m_model->houseEvents().find(arg)->get(),
                                  m_parent);
        }
        Event *operator()(const mef::Gate *arg)
        {
            model::Gate *proxyEvent = m_model->gates().find(arg)->get();
            if (auto it = ext::find(*m_transfer, arg)) {
                it->second->addTransferOut();
                return new TransferIn(proxyEvent, m_parent);
            }
            auto *arg_gate =
                new Gate(proxyEvent, m_model, m_transfer, m_parent);
            m_transfer->emplace(arg, arg_gate);
            return arg_gate;
        }

        QGraphicsItem *m_parent;
        decltype(model) m_model;
        decltype(transfer) m_transfer;
    } formula_visitor{this, model, transfer};
    double linkY = (m_size.height() - 1) * units().height();
    std::vector<std::pair<Event *, QGraphicsLineItem *>> children;
    for (const mef::Formula::Arg &arg : event->args()) {
        GUI_ASSERT(!arg.complement, );
        auto *child = std::visit(formula_visitor, arg.event);
        auto *link = new QGraphicsLineItem(0, 0, 0, units().height(), this);
        if (!children.empty())
            m_width += m_space * units().height();
        child->moveBy(m_width + child->width() / 2,
                      m_size.height() * units().height());
        link->moveBy(m_width + child->width() / 2, linkY);
        m_width += child->width();
        children.emplace_back(child, link);
    }
    // Shift the children left.
    for (auto &child : children) {
        child.first->moveBy(-m_width / 2, 0);
        child.second->moveBy(-m_width / 2, 0);

        // copy the line to connectingPath and delete the original one
        // so we can control its style
        QLineF currentLine = child.second->line();
        QPointF basePos = child.second->pos();
        connectingPath.moveTo(basePos + currentLine.p1());
        connectingPath.lineTo(basePos + currentLine.p2());
        delete child.second;
    }

    // Add the planar line to complete the connection.
    if (children.size() > 1) {
        connectingPath.moveTo(children.front().first->pos().x(), linkY);
        connectingPath.lineTo(children.back().first->pos().x(), linkY);
    }
    connectingLines = new QGraphicsPathItem(connectingPath);
    connectingLines->setParentItem(this);
}

std::unique_ptr<QAbstractGraphicsShapeItem>
Gate::getGateGraphicsType(mef::Connective type)
{
    static_assert(mef::kNumConnectives > 8, "Unexpected connective changes");
    switch (type) {
    case mef::kNull: {
        QPainterPath paintPath;
        paintPath.moveTo(0, 0);
        paintPath.lineTo(0, m_maxSize.height() * units().height());
        return std::make_unique<QGraphicsPathItem>(paintPath);
    }
    case mef::kAnd: {
        double h = m_maxSize.height() * units().height();
        QPainterPath paintPath;
        double maxHeight = m_maxSize.height() * units().height();
        paintPath.moveTo(0, maxHeight);
        paintPath.arcTo(-h / 2, 0, h, maxHeight * 2, 0, 180);
        paintPath.closeSubpath();
        return std::make_unique<QGraphicsPathItem>(paintPath);
    }
    case mef::kOr: {
        QPainterPath paintPath;
        double x1 = m_maxSize.width() * units().width() / 2;
        double maxHeight = m_maxSize.height() * units().height();
        QRectF rectangle(-x1, 0, x1 * 2, maxHeight * 2);
        paintPath.arcMoveTo(rectangle, 0);
        paintPath.arcTo(rectangle, 0, 180);
        double lowerArc = 0.25;
        rectangle.setHeight(rectangle.height() * lowerArc);
        rectangle.moveTop(maxHeight * (1 - lowerArc));
        paintPath.arcMoveTo(rectangle, 0);
        paintPath.arcTo(rectangle, 0, 180);
        paintPath.arcMoveTo(rectangle, 90);
        paintPath.lineTo(0, maxHeight);
        return std::make_unique<QGraphicsPathItem>(paintPath);
    }
    case mef::kAtleast: {
        double h = m_maxSize.height() * units().height();
        double a = h / sqrt(3);
        auto polygon =
            std::make_unique<QGraphicsPolygonItem>(QPolygonF{{{-a / 2, 0},
                                                              {a / 2, 0},
                                                              {a, h / 2},
                                                              {a / 2, h},
                                                              {-a / 2, h},
                                                              {-a, h / 2}}});
        auto *gate = static_cast<model::Gate *>(m_event);
        auto *text = new QGraphicsTextItem(QStringLiteral("%1/%2")
                                               .arg(gate->minNumber().value())
                                               .arg(gate->numArgs()),
                                           polygon.get());
        QFont font = text->font();
        font.setPointSizeF(1.5 * font.pointSizeF());
        text->setFont(font);
        text->setPos(-text->boundingRect().width() / 2,
                     (h - text->boundingRect().height()) / 2);
        return std::move(polygon);
    }
    case mef::kNot: {
        double h = m_maxSize.height() * units().height();
        QPainterPath paintPath;
        paintPath.addEllipse(-units().height() / 2, 0, units().height(),
                             units().height());
        paintPath.moveTo(0, units().height());
        double a = h - units().height();
        paintPath.lineTo(-a / 2, h);
        paintPath.lineTo(a / 2, h);
        paintPath.closeSubpath();
        return std::make_unique<QGraphicsPathItem>(paintPath);
    }
    case mef::kXor: {
        auto orItem = getGateGraphicsType(mef::kOr);
        double x1 = m_maxSize.width() * units().width() / 2;
        double h = m_maxSize.height() * units().height();
        QPainterPath paintPath;
        paintPath.lineTo(-x1, h);
        paintPath.moveTo(x1, h);
        paintPath.lineTo(0, 0);
        new QGraphicsPathItem(paintPath, orItem.get());
        return orItem;
    }
    case mef::kNor: {
        auto orItem = getGateGraphicsType(mef::kOr);
        auto circle = std::make_unique<QGraphicsEllipseItem>(
            -units().height() / 2, 0, units().height(), units().height());
        double orHeight = orItem->boundingRect().height();
        orItem->setScale((orHeight - units().height()) / orHeight);
        orItem->setPos(0, units().height());
        orItem.release()->setParentItem(circle.get());
        return std::move(circle);
    }
    case mef::kNand: {
        auto andItem = getGateGraphicsType(mef::kAnd);
        auto circle = std::make_unique<QGraphicsEllipseItem>(
            -units().height() / 2, 0, units().height(), units().height());
        double andHeight = andItem->boundingRect().height();
        andItem->setScale((andHeight - units().height()) / andHeight);
        andItem->setPos(0, units().height());
        andItem.release()->setParentItem(circle.get());
        return std::move(circle);
    }
    default:
        GUI_ASSERT(false && "Unexpected gate type", nullptr);
    }
}

double Gate::width() const
{
    return m_width;
}

void Gate::addTransferOut()
{
    if (m_transferOut)
        return;
    m_transferOut = true;
    QPainterPath paintPath;
    double x1 = m_maxSize.width() * units().width() / 2;
    double h = units().height() * std::sqrt(3) / 2;
    paintPath.lineTo(x1 + units().height(), 0);
    paintPath.lineTo(x1 + 0.5 * units().height(), h);
    paintPath.lineTo(x1 + 1.5 * units().height(), h);
    paintPath.lineTo(x1 + units().height(), 0);
    new QGraphicsPathItem(paintPath, Event::getTypeGraphics());
}

void Gate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    // change connecting line property
    QPen newPen(painter->pen());
    newPen.setWidthF(lineWidth);
    connectingLines->setPen(newPen);

    // call origin paint function
    Event::paint(painter, option, widget);
}

DiagramScene::DiagramScene(model::Gate *event, model::Model *model,
                           QObject *parent)
    : QGraphicsScene(parent), m_root(event), m_model(model)
{
    redraw();
    connect(m_model, OVERLOAD(model::Model, removed, model::Gate *), this,
            [this](model::Gate *gate) {
                if (gate == m_root) {
                    clear();
                    m_root = nullptr; ///< @todo Remove the implicit delete.
                }
            });
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
    const QList<QGraphicsItem *> items = selectedItems();
    if (!items.empty()) {
        GUI_ASSERT(items.size() == 1, );
        auto *event = dynamic_cast<diagram::Event *>(items.front());
        GUI_ASSERT(event, );
        emit activated(event->data());
    }
}

void DiagramScene::redraw()
{
    if (m_root == nullptr)
        return;

    clear();
    std::unordered_map<const mef::Gate *, Gate *> transfer;
    addItem(new Gate(m_root, m_model, &transfer));

    struct
    {
        void operator()(mef::Event *) const {}
        void operator()(mef::BasicEvent *event) const
        {
            auto *proxy = self->m_model->basicEvents().find(event)->get();
            connect(proxy, &model::BasicEvent::flavorChanged, self,
                    &DiagramScene::redraw, Qt::UniqueConnection);
        }
        DiagramScene *self;
    } visitor{this};

    auto link = [this, &visitor](model::Gate *gate) {
        connect(gate, &model::Gate::formulaChanged, this, &DiagramScene::redraw,
                Qt::UniqueConnection);
        for (const mef::Formula::Arg &arg : gate->args()) {
            GUI_ASSERT(!arg.complement, );
            std::visit(visitor, arg.event);
        }
    };

    /// @todo Finer signal tracking.
    link(m_root);
    for (const auto &entry : transfer)
        link(m_model->gates().find(entry.first)->get());

    // adjust the scene coverage so there will be appropriate spacing around the
    // tree
    setSceneRect(QRectF()); // set a null QRectF to the scence, so later
                            // sceneRect() would return a QRectF object
                            // corresponds the real bounding box
    QRectF contentRec = sceneRect();
    contentRec.adjust(-5, -20, 5, 100); // hard code: adjust the sceneRect
    setSceneRect(contentRec);
}

} // namespace scram::gui::diagram
