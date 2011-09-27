/********************************************************************
 *             a  A                                                        
 *            AM\/MA                                                         
 *           (MA:MMD                                                         
 *            :: VD
 *           ::  º                                                         
 *          ::                                                              
 *         ::   **      .A$MMMMND   AMMMD     AMMM6    MMMM  MMMM6             
 +       6::Z. TMMM    MMMMMMMMMDA   VMMMD   AMMM6     MMMMMMMMM6            
 *      6M:AMMJMMOD     V     MMMA    VMMMD AMMM6      MMMMMMM6              
 *      ::  TMMTMC         ___MMMM     VMMMMMMM6       MMMM                   
 *     MMM  TMMMTTM,     AMMMMMMMM      VMMMMM6        MMMM                  
 *    :: MM TMMTMMMD    MMMMMMMMMM       MMMMMM        MMMM                   
 *   ::   MMMTTMMM6    MMMMMMMMMMM      AMMMMMMD       MMMM                   
 *  :.     MMMMMM6    MMMM    MMMM     AMMMMMMMMD      MMMM                   
 *         TTMMT      MMMM    MMMM    AMMM6  MMMMD     MMMM                   
 *        TMMMM8       MMMMMMMMMMM   AMMM6    MMMMD    MMMM                   
 *       TMMMMMM$       MMMM6 MMMM  AMMM6      MMMMD   MMMM                   
 *      TMMM MMMM                                                           
 *     TMMM  .MMM                                         
 *     TMM   .MMD       ARBITRARY·······XML········RENDERING                           
 *     TMM    MMA       ====================================                              
 *     TMN    MM                               
 *      MN    ZM                       
 *            MM,
 *
 * 
 *      AUTHORS: Miro Keller
 *      
 *      COPYRIGHT: ©2011 - All Rights Reserved
 *
 *      LICENSE: see License.txt file
 *
 *      WEB: http://axr.vg
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************
 *
 *      FILE INFORMATION:
 *      =================
 *      Last changed: 2011/09/26
 *      HSS version: 1.0
 *      Core version: 0.3
 *      Revision: 27
 *
 ********************************************************************/

#include "HSSDisplayObject.h"
#include "../../axr/AXRDebugging.h"
#include <math.h>
#include <boost/pointer_cast.hpp>
#include <cairo/cairo.h>
#include "../parsing/HSSParserNode.h"
#include "../parsing/HSSExpression.h"
#include "../parsing/HSSConstants.h"
#include "../parsing/HSSObjectDefinition.h"
#include "HSSContainer.h"
#include <sstream>
#include <string>
#include "../../axr/AXRController.h"
#include <pango/pangocairo.h>
#include "HSSRgba.h"

using namespace AXR;

bool HSSDisplayObject::isKeyword(std::string value, std::string property)
{
    if (   property == "anchorX"
        || property == "anchorY"
        || property == "alignX"
        || property == "alignY"
        ) {
        if (value == "center"){
            return true;
        }
    } else if (property == "flow") {
        if (value == "yes" || value == "no"){
            return true;
        }
    } else if (property == "font") {
        if (   value == "thin"
            || value == "ultralight"
            || value == "light"
            || value == "normal"
            || value == "medium"
            || value == "semibold"
            || value == "bold"
            || value == "ultrabold"
            || value == "heavy"
            || value == "ultraheavy"){
            return true;
        }
    }
    
    //if we reached this far, let the superclass handle it
    return HSSObject::isKeyword(value, property);
}

HSSDisplayObject::HSSDisplayObject()
: HSSObject()
{
    std_log1(std::string("creating annonymous display object"));
    this->type = HSSObjectTypeDisplayObject;
    
    this->_isDirty= true;
    this->_needsRereadRules = true;
    this->_needsSurface = true;
    this->_needsLayout = true;
    this->backgroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    this->foregroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    this->bordersSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    
    x = y = globalX = globalY = width = height = anchorX = anchorY = alignX = alignY = 0.;
    drawIndex = 0;
    tabIndex = zoomFactor = 1;
    flow = visible = true;
    //fixme: change to camelCase
    does_float = false;
    this->heightByContent = false;
    
    elementName = std::string();
    contentText = std::string();
    
    observedWidth = observedHeight
    = observedAnchorX = observedAnchorY
    = observedAlignX = observedAlignY
    = NULL;
    
    this->dBackground = HSSMultipleValue();
    this->dFont = HSSMultipleValue();
}

HSSDisplayObject::HSSDisplayObject(std::string name)
: HSSObject(name)
{
    std_log1(std::string("creating display object with name ").append(name));   
    this->type = HSSObjectTypeDisplayObject;
    
    this->_isDirty = true;
    this->_needsRereadRules = true;
    this->_needsSurface = true;
    this->_needsLayout = true;
    this->backgroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    this->foregroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    this->bordersSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    
    x = y = globalX = globalY = width = height = anchorX = anchorY = alignX = alignY = 0.;
    drawIndex = 0;
    tabIndex = zoomFactor = 1;
    flow = visible = true;
    //fixme: change to camelCase
    does_float = false;
    this->heightByContent = false;
    
    elementName = std::string();
    contentText = std::string();
    
    observedWidth = observedHeight
    = observedAnchorX = observedAnchorY
    = observedAlignX = observedAlignY
    = NULL;
    
    this->dBackground = HSSMultipleValue();
}

HSSDisplayObject::~HSSDisplayObject()
{
    std_log1(std::string("destroying display object with name ").append(this->name));
    cairo_surface_destroy(this->backgroundSurface);
    cairo_surface_destroy(this->foregroundSurface);
    cairo_surface_destroy(this->bordersSurface);
}

std::string HSSDisplayObject::toString()
{
    if (this->isNamed()) {
        return std::string("HSSDisplayObject: ").append(this->name);
    } else {
        return "Annonymous HSSDisplayObject";
    }
}

bool HSSDisplayObject::canHaveChildren()
{
    return false;
}

std::string HSSDisplayObject::defaultObjectType(std::string property)
{
    if (property == "margin"){
        return "margin";
    } else if (property == "border"){
        return "lineBorder";
    } else if (property == "transform"){
        return "rotate";
    } else if (property == "effects"){
        return "shadow";
    } else if (property == "animation") {
        return "transition";
    } else if (property == "behavior") {
        return "click";
    } else if (property == "mask") {
        return "image";
    } else if (property == "font") {
        return "font";    
    } else {
        return HSSObject::defaultObjectType(property);
    }
}

boost::shared_ptr<HSSContainer> HSSDisplayObject::getParent()
{
    boost::shared_ptr<HSSContainer> parent = this->parent.lock();
    return parent;
}

void HSSDisplayObject::setParent(boost::shared_ptr<HSSContainer> parent)
{
    this->parent = parentPointer(parent);
}


void HSSDisplayObject::attributesAdd(std::string name, std::string value)
{
    this->attributes[name] = value;
}

void HSSDisplayObject::attributesRemove(std::string name)
{
    this->attributes.erase(name);
}

std::string HSSDisplayObject::getContentText()
{
    return this->contentText;
}

void HSSDisplayObject::setContentText(std::string text)
{
    this->contentText = text;
}

void HSSDisplayObject::appendContentText(std::string text)
{
    this->contentText.append(text);
}

std::string HSSDisplayObject::getElementName()
{
    return this->elementName;
}

void HSSDisplayObject::setElementName(std::string newName)
{
    this->elementName = newName;
}

//rules

void HSSDisplayObject::rulesAdd(HSSRule::p newRule)
{
    this->rules.push_back(newRule);
}

HSSRule::p HSSDisplayObject::rulesGet(unsigned index)
{
    return this->rules[index];
}

void HSSDisplayObject::rulesRemove(unsigned index)
{
    this->rules.erase(this->rules.begin()+index);
}

void HSSDisplayObject::rulesRemoveLast()
{
    this->rules.pop_back();
}

const int HSSDisplayObject::rulesSize()
{
    return this->rules.size();
}

void HSSDisplayObject::readDefinitionObjects()
{
//    if(this->_needsRereadRules){
        unsigned i, j;
    
        this->setDefaults();
        
        std::string propertyName;
        for (i=0; i<this->rules.size(); i++) {
            HSSRule::p& rule = this->rules[i];
            
            for (j=0; j<rule->propertiesSize(); j++) {
                HSSPropertyDefinition::p& propertyDefinition = rule->propertiesGet(j);
                propertyName = propertyDefinition->getName();
                
                if(propertyName == "width"){
                    //store a copy of the value
                    this->setDWidth(propertyDefinition->getValue());
                    
                } else if(propertyName == "height"){
                    //store a copy of the value
                    this->setDHeight(propertyDefinition->getValue());
                } else if(propertyName == "anchorX"){
                    //store a copy of the value
                    this->setDAnchorX(propertyDefinition->getValue());
                } else if(propertyName == "anchorY"){
                    //store a copy of the value
                    this->setDAnchorY(propertyDefinition->getValue());
                } else if(propertyName == "alignX"){
                    //store a copy of the value
                    this->setDAlignX(propertyDefinition->getValue());
                } else if(propertyName == "alignY"){
                    //store a copy of the value
                    this->setDAlignY(propertyDefinition->getValue());
                } else if(propertyName == "background"){
                    //store a copy of the value
                    this->setDBackground(propertyDefinition->getValue());
                } else if(propertyName == "font"){
                    //store a copy of the value
                    this->setDFont(propertyDefinition->getValue());
                }
            }
        }
        
        this->_needsRereadRules = false;
//    }
}

void HSSDisplayObject::setNeedsRereadRules(bool value)
{
    this->_needsRereadRules = value;
}

bool HSSDisplayObject::needsRereadRules()
{
    return this->_needsRereadRules;
}

void HSSDisplayObject::recursiveReadDefinitionObjects()
{
    this->readDefinitionObjects();
}

void HSSDisplayObject::regenerateSurfaces()
{
//    if(this->_needsSurface){
        cairo_surface_destroy(this->backgroundSurface);
        cairo_surface_destroy(this->foregroundSurface);
        cairo_surface_destroy(this->bordersSurface);
        this->backgroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ceil(this->width), ceil(this->height));
        this->foregroundSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ceil(this->width), ceil(this->height));
        this->bordersSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ceil(this->width), ceil(this->height));
        this->setDirty(true);
        this->_needsSurface = false;
#if AXR_DEBUG_LEVEL > 0
        std::ostringstream wstr;
        wstr << this->width;
        std::ostringstream hstr;
        hstr << this->height;
        std_log1("created a new surface width:"+wstr.str()+" height:"+hstr.str());
#endif
//    }
}

void HSSDisplayObject::recursiveRegenerateSurfaces()
{
    this->regenerateSurfaces();
}


void HSSDisplayObject::setNeedsSurface(bool value)
{
    this->_needsSurface = value;
}

bool HSSDisplayObject::needsSurface()
{
    return this->_needsSurface;
}


void HSSDisplayObject::setDirty(bool value)
{
    //std_log1("setting "+this->name+" dirty");
    this->_isDirty = value;
}

bool HSSDisplayObject::isDirty()
{
    return this->_isDirty;
}

void HSSDisplayObject::draw(cairo_t * cairo)
{
    std_log1("drawing "+this->elementName);
    if(this->_needsRereadRules){
        std_log1("rereading rules of "+this->elementName);
        this->readDefinitionObjects();
    }
    if(this->_needsLayout){
        std_log1("recalculating layout of "+this->elementName);
        this->layout();
    }
    if(this->_needsSurface){
        std_log1("regenerating surfaces of "+this->elementName);
        this->regenerateSurfaces();
    }
    if(this->_isDirty){
#if AXR_DEBUG_LEVEL > 0
        std::ostringstream xStream;
        xStream << this->x;
        std::ostringstream yStream;
        yStream << this->y;
        
        std_log1("redrawing contents of "+this->elementName+" with x: "+xStream.str()+" and y: "+yStream.str());
#endif
        this->_isDirty = false;
        this->drawBackground();
        this->drawForeground();
        this->drawBorders();
    }
    
    cairo_set_source_surface(cairo, this->backgroundSurface, this->globalX, this->globalY);
    cairo_paint(cairo);
    cairo_set_source_surface(cairo, this->foregroundSurface, this->globalX, this->globalY);
    cairo_paint(cairo);
    cairo_set_source_surface(cairo, this->bordersSurface, this->globalX, this->globalY);
    cairo_paint(cairo);
}

void HSSDisplayObject::drawBackground()
{
    long double r = 0., g = 0., b = 0., a = 0;
    if(this->backgroundColor){
        r = this->backgroundColor->getRed();
        g = this->backgroundColor->getGreen();
        b = this->backgroundColor->getBlue();
        a = this->backgroundColor->getAlpha();
    }

    cairo_t * cairo = cairo_create(this->backgroundSurface);
    cairo_rectangle(cairo, 0., 0., this->width, this->height);
    cairo_set_source_rgba(cairo, (r/255), (g/255), (b/255), (a/255));
    cairo_fill(cairo);
    
    //draw a small rectangle to be able to see the anchor X and Y
//    cairo_rectangle(cairo, this->anchorX-1., this->anchorY-1., 2, 2);
//    cairo_set_source_rgb(cairo, 1,0,0);
//    cairo_fill(cairo);
    
    cairo_destroy(cairo);
}

void HSSDisplayObject::drawForeground()
{
    cairo_t * cairo = cairo_create(this->foregroundSurface);
    
    PangoLayout *layout;
    PangoFontDescription *font_description;
    
    
    layout = pango_cairo_create_layout (cairo);
    pango_layout_set_width(layout, this->width * PANGO_SCALE);
    
    font_description = pango_font_description_new ();
    
    if (this->font){
        pango_font_description_set_family (font_description, this->font->getFace().c_str());
        
        pango_font_description_set_weight (font_description, PANGO_WEIGHT_NORMAL);
        pango_font_description_set_absolute_size (font_description, this->font->getSize() * PANGO_SCALE);
        
        if (this->font->getColor()){
            HSSRgba::p textColor = boost::static_pointer_cast<HSSRgba>(this->font->getColor());
            cairo_set_source_rgb (cairo, textColor->getRed()/255, textColor->getGreen()/255, textColor->getBlue()/255);
        } else {
            cairo_set_source_rgb (cairo, 0, 0, 0);
        }
        
    } else {
        pango_font_description_set_family (font_description, "monospace");
        pango_font_description_set_weight (font_description, PANGO_WEIGHT_NORMAL);
        pango_font_description_set_absolute_size (font_description, 12 * PANGO_SCALE);
        cairo_set_source_rgb (cairo, 0, 0, 0);
    }
    
    pango_layout_set_font_description (layout, font_description);
    pango_layout_set_text (layout, this->getContentText().c_str(), -1);
    
    
    cairo_move_to (cairo, 0, 0);
    pango_cairo_show_layout (cairo, layout);
    
    g_object_unref (layout);
    pango_font_description_free (font_description);
    
    
    
    
    
    
//    cairo_t * cairo = cairo_create(this->foregroundSurface);
//    cairo_select_font_face (cairo, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//    cairo_set_font_size (cairo, 12.0);
//    cairo_set_source_rgb (cairo, 0.0, 0.0, 0.0);
//    cairo_move_to (cairo, 0.0, 12.0);
//    std_log1(this->getContentText());
//    cairo_show_text (cairo, this->getContentText().c_str());
    
    cairo_destroy(cairo);
}

void HSSDisplayObject::drawBorders()
{
//    cairo_t * cairo = cairo_create(this->bordersSurface);
//    cairo_rectangle(cairo, 0., 0., this->width, this->height);
//    cairo_set_source_rgb(cairo, 0,0,0);
//    cairo_stroke(cairo);
//    
//    cairo_destroy(cairo);
}

void HSSDisplayObject::recursiveDraw(cairo_t * cairo)
{
    this->draw(cairo);
}

//layout
void HSSDisplayObject::setNeedsLayout(bool value)
{
    this->_needsLayout = value;
}

const bool HSSDisplayObject::needsLayout() const
{
    return this->_needsLayout;
}

void HSSDisplayObject::layout()
{
    //do nothing
}

void HSSDisplayObject::recursiveLayout()
{
    this->layout();
}

//width
HSSParserNode::p HSSDisplayObject::getDWidth()  {   return this->dWidth; }
void HSSDisplayObject::setDWidth(HSSParserNode::p value)
{
    
    if(value->isA(HSSParserNodeTypeKeywordConstant)){
        
    } else {
        this->dWidth = value;
        HSSObservableProperty observedProperty = HSSObservablePropertyWidth;
        if(this->observedWidth != NULL)
        {
            this->observedWidth->removeObserver(this->observedWidthProperty, HSSObservablePropertyWidth, this);
        }
        HSSContainer::p parentContainer = this->getParent();
        if(parentContainer){
            this->width = this->_setLDProperty(
                                               &HSSDisplayObject::widthChanged,
                                               value,
                                               parentContainer->width,
                                               observedProperty,
                                               parentContainer.get(),
                                               HSSObservablePropertyWidth,
                                               this->observedWidth,
                                               this->observedWidthProperty,
                                               &(parentContainer->getChildren())
                                               );
        } else {
            this->width = this->_setLDProperty(
                                               NULL,
                                               value,
                                               0,
                                               observedProperty,
                                               this,
                                               HSSObservablePropertyWidth,
                                               this->observedWidth,
                                               this->observedWidthProperty,
                                               NULL
                                               );
        }
        
        
        this->notifyObservers(HSSObservablePropertyWidth, &this->width);
        this->setNeedsSurface(true);
        this->setNeedsLayout(true);
        this->setDirty(true);
    }

    
//    //std_log1("setting width of "+this->name+":");
//    this->dWidth = value;
//    HSSParserNodeType nodeType = value->getType();
//    
//    if(observedWidth != NULL)
//    {
//        //std_log1("#################should unsubscribe observer here#########################");
//        observedWidth->removeObserver(HSSObservablePropertyWidth, this);
//    }
//    
//    switch (nodeType) {
//        case HSSParserNodeTypeNumberConstant:
//        {
//            HSSNumberConstant::p widthValue = boost::static_pointer_cast<HSSNumberConstant>(value);
//            this->width = widthValue->getValue();
//            break;
//        }
//            
//        case HSSParserNodeTypePercentageConstant:
//        {
//            HSSPercentageConstant::p widthValue = boost::static_pointer_cast<HSSPercentageConstant>(value);
//            HSSContainer::p parentContainer = this->getParent();
//            if(parentContainer){
//                this->width = widthValue->getValue(parentContainer->width);
//                parentContainer->observe(HSSObservablePropertyWidth, this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::widthChanged));
//                this->observedWidth = parentContainer.get();
//                
//            } else {
//                this->width = widthValue->getValue(this->width);
//            }
//            break;
//        }
//            
//        case HSSParserNodeTypeExpression:
//        {
//            HSSExpression::p widthExpression = boost::static_pointer_cast<HSSExpression>(value);
//            HSSContainer::p parentContainer = this->getParent();
//            if(parentContainer){
//                widthExpression->setPercentageBase(parentContainer->width);
//                widthExpression->setPercentageObserved(HSSObservablePropertyWidth, parentContainer.get());
//                widthExpression->setScope(&(parentContainer->getChildren()));
//                this->width = widthExpression->evaluate();
//                widthExpression->observe(HSSObservablePropertyWidth, this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::widthChanged));
//            } else {
//                widthExpression->setPercentageBase(this->width);
//                widthExpression->setPercentageObserved(HSSObservablePropertyWidth, this);
//                if(this->isA(HSSObjectTypeContainer)){
//                    HSSContainer * thisContainer = static_cast<HSSContainer *>(this);
//                    widthExpression->setScope(&(thisContainer->getChildren()));
//                }
//                this->width = widthExpression->evaluate();
//                widthExpression->observe(HSSObservablePropertyWidth, this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::widthChanged));
//            }
//            break; 
//        }
//            
//        case HSSParserNodeTypeKeywordConstant:
//            
//            break;
//            
//        default:
//            throw "unknown parser node type while setting dWidth";
//            break;
//    }
//    
//    //fprintf(stderr, "%f\n", this->width);
//    this->notifyObservers(HSSObservablePropertyWidth, &this->width);
//    this->setNeedsSurface(true);
//    this->setDirty(true);
}

void HSSDisplayObject::widthChanged(HSSObservableProperty source, void*data)
{
    HSSParserNodeType nodeType = this->dWidth->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p widthValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dWidth);
            this->width = widthValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p widthExpression = boost::static_pointer_cast<HSSExpression>(this->dWidth);
            this->width = widthExpression->evaluate();
            break;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyWidth, &this->width);
    this->setNeedsSurface(true);
    this->setNeedsLayout(true);
    this->setDirty(true);
}

//height
HSSParserNode::p HSSDisplayObject::getDHeight() { return this->dHeight; }
void HSSDisplayObject::setDHeight(HSSParserNode::p value)
{
    if(value->isA(HSSParserNodeTypeKeywordConstant)){
        this->heightByContent = true;
        
    } else {
        this->dHeight = value;
        this->heightByContent = false;
        HSSObservableProperty observedProperty = HSSObservablePropertyHeight;
        if(this->observedHeight != NULL)
        {
            this->observedHeight->removeObserver(this->observedHeightProperty, HSSObservablePropertyHeight, this);
        }
        HSSContainer::p parentContainer = this->getParent();
        if(parentContainer){
            this->height = this->_setLDProperty(
                                                &HSSDisplayObject::heightChanged,
                                                value,
                                                parentContainer->height,
                                                observedProperty,
                                                parentContainer.get(),
                                                HSSObservablePropertyHeight,
                                                this->observedHeight,
                                                this->observedHeightProperty,
                                                &(parentContainer->getChildren())
                                                );
        } else {
            this->height = this->_setLDProperty(
                                                NULL,
                                                value,
                                                150,
                                                observedProperty,
                                                this,
                                                HSSObservablePropertyHeight,
                                                this->observedHeight,
                                                this->observedHeightProperty,
                                                NULL
                                                );
        }
        
        this->notifyObservers(HSSObservablePropertyHeight, &this->height);
        this->setNeedsSurface(true);
        this->setNeedsLayout(true);
        this->setDirty(true);
    }
}

void HSSDisplayObject::heightChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dHeight->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p heightValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dHeight);
            this->height = heightValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p heightExpression = boost::static_pointer_cast<HSSExpression>(this->dHeight);
            this->height = heightExpression->evaluate();
            break;
        }
            
        case HSSParserNodeTypeKeywordConstant:
        {
            this->height = *(long double*)data;
            break;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyHeight, &this->height);
    this->setNeedsSurface(true);
    this->setNeedsLayout(true);
    this->setDirty(true);
}

//anchorX
HSSParserNode::p HSSDisplayObject::getDAnchorX() { return this->dAnchorX; }
void HSSDisplayObject::setDAnchorX(HSSParserNode::p value)
{
    this->dAnchorX = value;
    HSSObservableProperty observedProperty = HSSObservablePropertyWidth;
    if(this->observedAnchorX != NULL)
    {
        this->observedAnchorX->removeObserver(this->observedAnchorXProperty, HSSObservablePropertyAnchorX, this);
    }
    HSSContainer::p parentContainer = this->getParent();
    const std::vector<HSSDisplayObject::p> * scope;
    if(parentContainer){
        scope = &(parentContainer->getChildren());
    } else {
        scope = NULL;
    }
    this->anchorX = this->_setLDProperty(
                                         &HSSDisplayObject::anchorXChanged,
                                         value,
                                         this->width,
                                         observedProperty,
                                         this,
                                         HSSObservablePropertyAnchorX,
                                         this->observedAnchorX,
                                         this->observedAnchorXProperty,
                                         scope
                                         );
    this->notifyObservers(HSSObservablePropertyAnchorX, &this->anchorX);
#if AXR_DEBUG_LEVEL > 0
    this->setDirty(true);
#endif
}

void HSSDisplayObject::anchorXChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dAnchorX->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p percentageValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dAnchorX);
            this->anchorX = percentageValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p expressionValue = boost::static_pointer_cast<HSSExpression>(this->dAnchorX);
            this->anchorX = expressionValue->evaluate();
            break;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyAnchorX, &this->anchorX);
#if AXR_DEBUG_LEVEL > 0
    this->setDirty(true);
#endif
}


//anchorY
HSSParserNode::p HSSDisplayObject::getDAnchorY() { return this->dAnchorY; }
void HSSDisplayObject::setDAnchorY(HSSParserNode::p value)
{
    this->dAnchorY = value;
    HSSObservableProperty observedProperty = HSSObservablePropertyHeight;
    if(this->observedAnchorY != NULL)
    {
        this->observedAnchorY->removeObserver(this->observedAnchorYProperty, HSSObservablePropertyAnchorY, this);
    }
    HSSContainer::p parentContainer = this->getParent();
    const std::vector<HSSDisplayObject::p> * scope;
    if(parentContainer){
        scope = &(parentContainer->getChildren());
    } else {
        scope = NULL;
    }
    this->anchorY = this->_setLDProperty(
                                         &HSSDisplayObject::anchorYChanged,
                                         value,
                                         this->height,
                                         observedProperty,
                                         this,
                                         HSSObservablePropertyAnchorY,
                                         this->observedAnchorY,
                                         this->observedAnchorYProperty,
                                         scope
                                         );
    this->notifyObservers(HSSObservablePropertyAnchorY, &this->anchorY);
#if AXR_DEBUG_LEVEL > 0
    this->setDirty(true);
#endif
}

void HSSDisplayObject::anchorYChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dAnchorY->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p percentageValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dAnchorY);
            this->anchorY = percentageValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p expressionValue = boost::static_pointer_cast<HSSExpression>(this->dAnchorY);
            this->anchorY = expressionValue->evaluate();
            break;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyAnchorY, &this->anchorY);
#if AXR_DEBUG_LEVEL > 0
    this->setDirty(true);
#endif
}


//flow
HSSParserNode::p HSSDisplayObject::getDFlow() { return this->dFlow; }
void HSSDisplayObject::setDFlow(HSSParserNode::p value)
{
    this->dFlow = value;
}

//alignX
HSSParserNode::p HSSDisplayObject::getDAlignX() { return this->dAlignX; }
void HSSDisplayObject::setDAlignX(HSSParserNode::p value)
{
    this->dAlignX = value;
    if(this->observedAlignX != NULL)
    {
        this->observedAlignX->removeObserver(this->observedAlignXProperty, HSSObservablePropertyAlignX, this);
    }
    
    if(value->isA(HSSParserNodeTypeKeywordConstant)){
        
        HSSKeywordConstant::p keywordValue = boost::static_pointer_cast<HSSKeywordConstant>(value);
        if(keywordValue->getValue() == "auto"){
            HSSContainer::p parentContainer = this->getParent();
            if(parentContainer){
                this->alignX = parentContainer->contentAlignX;
                parentContainer->observe(HSSObservablePropertyContentAlignX, HSSObservablePropertyAlignX, this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::alignXChanged));
                this->observedAlignX = parentContainer.get();
                this->observedAlignXProperty = HSSObservablePropertyContentAlignX;
            } else {
                this->alignX = 0;
            }
//            this->alignX = 0;
            
        } else {
            std_log1("any keyword other than auto has not been implemented yet");
            throw;
        }
        
    } else {
        HSSObservableProperty observedProperty = HSSObservablePropertyWidth;
        HSSContainer::p parentContainer = this->getParent();
        if(parentContainer){
            this->alignX = this->_setLDProperty(
                                               &HSSDisplayObject::alignXChanged,
                                               value,
                                               parentContainer->width,
                                               observedProperty,
                                               parentContainer.get(),
                                               HSSObservablePropertyAlignX,
                                               this->observedAlignX,
                                               this->observedAlignXProperty,
                                               &(parentContainer->getChildren())
                                               );
            parentContainer->setNeedsLayout(true);
        } else {
            this->alignX = this->_setLDProperty(
                                               NULL,
                                               value,
                                               0,
                                               observedProperty,
                                               this,
                                               HSSObservablePropertyAlignX,
                                               this->observedAlignX,
                                               this->observedAlignXProperty,
                                               NULL
                                               );
        }
    }
    
    this->notifyObservers(HSSObservablePropertyAlignX, &this->alignX);
}

void HSSDisplayObject::alignXChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dAlignX->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p percentageValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dAlignX);
            this->alignX = percentageValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p expressionValue = boost::static_pointer_cast<HSSExpression>(this->dAlignX);
            this->alignX = expressionValue->evaluate();
            break;
        }
            
        case HSSParserNodeTypeKeywordConstant:
        {
            this->alignX = *(long double*)data;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyAlignX, &this->alignX);
}

//alignY
HSSParserNode::p HSSDisplayObject::getDAlignY() { return this->dAlignX; }
void HSSDisplayObject::setDAlignY(HSSParserNode::p value)
{
    this->dAlignY = value;
    if(this->observedAlignY != NULL)
    {
        this->observedAlignY->removeObserver(this->observedAlignYProperty, HSSObservablePropertyAlignY, this);
    }
    
    if(value->isA(HSSParserNodeTypeKeywordConstant)){
        
        HSSKeywordConstant::p keywordValue = boost::static_pointer_cast<HSSKeywordConstant>(value);
        if(keywordValue->getValue() == "auto"){
            HSSContainer::p parentContainer = this->getParent();
            if(parentContainer){
                this->alignY = parentContainer->contentAlignY;
                parentContainer->observe(HSSObservablePropertyContentAlignY, HSSObservablePropertyAlignY, this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::alignYChanged));
                this->observedAlignY = parentContainer.get();
                this->observedAlignYProperty = HSSObservablePropertyContentAlignY;
            } else {
                this->alignY = 0;
            }
            //            this->alignY = 0;
            
        } else {
            std_log1("any keyword other than auto has not been implemented yet");
            throw;
        }
        
    } else {
        HSSObservableProperty observedProperty = HSSObservablePropertyHeight;
        HSSContainer::p parentContainer = this->getParent();
        if(parentContainer){
            this->alignY = this->_setLDProperty(
                                                &HSSDisplayObject::alignYChanged,
                                                value,
                                                parentContainer->height,
                                                observedProperty,
                                                parentContainer.get(),
                                                HSSObservablePropertyAlignY,
                                                this->observedAlignY,
                                                this->observedAlignYProperty,
                                                &(parentContainer->getChildren())
                                                );
            parentContainer->setNeedsLayout(true);
        } else {
            this->alignY = this->_setLDProperty(
                                               NULL,
                                               value,
                                               0,
                                               observedProperty,
                                               this,
                                               HSSObservablePropertyAlignY,
                                               this->observedAlignY,
                                               this->observedAlignYProperty,
                                               NULL
                                               );
        }
    }
    
    this->notifyObservers(HSSObservablePropertyAlignY, &this->alignY);
}

void HSSDisplayObject::alignYChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dAlignY->getType();
    switch (nodeType) {
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p percentageValue = boost::static_pointer_cast<HSSPercentageConstant>(this->dAlignY);
            this->alignY = percentageValue->getValue(*(long double*)data);
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p expressionValue = boost::static_pointer_cast<HSSExpression>(this->dAlignY);
            this->alignY = expressionValue->evaluate();
            break;
        }
            
        case HSSParserNodeTypeKeywordConstant:
        {
            this->alignY = *(long double*)data;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyAlignY, &this->alignY);
}


//background
const HSSMultipleValue HSSDisplayObject::getDBackground() const { return this->dBackground; }
void HSSDisplayObject::setDBackground(HSSParserNode::p value)
{
    HSSMultipleValue newBackground;
    newBackground.add(value);
    this->dBackground = newBackground;
    
    switch (value->getType()) {
        case HSSParserNodeTypeObjectDefinition:
        {
            HSSObjectDefinition::p objdef = boost::static_pointer_cast<HSSObjectDefinition>(value);
            this->backgroundColor = boost::static_pointer_cast<HSSRgba>(objdef->getObject());
            std_log1("added background rgba object: "+this->backgroundColor->toString());
            break;
        }
            
        case HSSParserNodeTypeObjectNameConstant:
        {
            try {
                HSSObjectNameConstant::p objdef = boost::static_pointer_cast<HSSObjectNameConstant>(value);
                this->backgroundColor = boost::static_pointer_cast<HSSRgba>(this->axrController->objectTreeGet(objdef->getValue()));
            } catch (HSSObjectNotFoundException * e) {
                std_log1(e->toString());
            }
            
            break;
        }
            
        default:
        {
            std_log1("unkown parser node type in background property of display object "+this->name+": "+HSSParserNode::parserNodeStringRepresentation(value->getType()));
            break;
        }
    }
    
    //this->notifyObservers(HSSObservablePropertyAlignY, &this->alignY);
}



//font
const HSSMultipleValue HSSDisplayObject::getDFont() const { return this->dFont; }
void HSSDisplayObject::setDFont(HSSParserNode::p value)
{
    HSSMultipleValue newFont;
    newFont.add(value);
    this->dFont = newFont;
    
    switch (value->getType()) {
        case HSSParserNodeTypeObjectDefinition:
        {
            HSSObjectDefinition::p objdef = boost::static_pointer_cast<HSSObjectDefinition>(value);
            this->font = boost::static_pointer_cast<HSSFont>(objdef->getObject());
            std_log1("added font object: "+this->font->toString());
            break;
        }
            
        case HSSParserNodeTypeObjectNameConstant:
        {
            try {
                HSSObjectNameConstant::p objdef = boost::static_pointer_cast<HSSObjectNameConstant>(value);
                this->font = boost::static_pointer_cast<HSSFont>(this->axrController->objectTreeGet(objdef->getValue()));
            } catch (HSSObjectNotFoundException * e) {
                std_log1(e->toString());
            }
            
            break;
        }
            
        default:
        {
            std_log1("unkown parser node type in font property of display object "+this->name+": "+HSSParserNode::parserNodeStringRepresentation(value->getType()));
            break;
        }
    }
    
    //this->notifyObservers(HSSObservablePropertyAlignY, &this->alignY);
}

void HSSDisplayObject::fontChanged(HSSObservableProperty source, void *data)
{
    HSSParserNodeType nodeType = this->dFont.last()->getType();
    switch (nodeType) {
        case HSSParserNodeTypeObjectDefinition:
        case HSSParserNodeTypeObjectNameConstant:
        {
            this->setDirty(true);
            break;
        }
            
        default:
            break;
    }
    
    this->notifyObservers(HSSObservablePropertyFont, &this->font);
}

//defaults
void HSSDisplayObject::setDefaults()
{
    //width
    HSSPercentageConstant::p newDWidth(new HSSPercentageConstant(100.));
    this->setDWidth(newDWidth);
    //height
    HSSKeywordConstant::p newDHeight(new HSSKeywordConstant("content"));
    this->setDHeight(newDHeight);
    //anchorX
    HSSPercentageConstant::p newDAnchorX(new HSSPercentageConstant(50));
    this->setDAnchorX(newDAnchorX);
    //anchorY
    HSSPercentageConstant::p newDAnchorY(new HSSPercentageConstant(50));
    this->setDAnchorY(newDAnchorY);
    //flow
    HSSKeywordConstant::p newDFlow(new HSSKeywordConstant("yes"));
    this->setDFlow(newDFlow);
    //alignX
    HSSKeywordConstant::p newDAlignX(new HSSKeywordConstant("auto"));
    this->setDAlignX(newDAlignX);
    //alignY
    HSSKeywordConstant::p newDAlignY(new HSSKeywordConstant("auto"));
    this->setDAlignY(newDAlignY);
    //background
    HSSKeywordConstant::p newDBackground(new HSSKeywordConstant("none"));
    this->setDBackground(newDBackground);
}


long double HSSDisplayObject::_setLDProperty(
                                             void(HSSDisplayObject::*callback)(HSSObservableProperty property, void* data),
                                             HSSParserNode::p       value,
                                             long double            percentageBase,
                                             HSSObservableProperty  observedProperty,
                                             HSSObservable *        observedObject,
                                             HSSObservableProperty  observedSourceProperty,
                                             HSSObservable *        &observedStore,
                                             HSSObservableProperty  &observedStoreProperty,
                                             const std::vector<HSSDisplayObject::p> * scope
                                             )
{
    long double ret;
    
    HSSParserNodeType nodeType = value->getType();
    switch (nodeType) {
        case HSSParserNodeTypeNumberConstant:
        {
            HSSNumberConstant::p numberValue = boost::static_pointer_cast<HSSNumberConstant>(value);
            ret = numberValue->getValue();
            observedStore = NULL;
            break;
        }
            
        case HSSParserNodeTypePercentageConstant:
        {
            HSSPercentageConstant::p percentageValue = boost::static_pointer_cast<HSSPercentageConstant>(value);
            ret = percentageValue->getValue(percentageBase);
            if(callback != NULL)
            {
                observedObject->observe(observedProperty, observedSourceProperty, this, new HSSValueChangedCallback<HSSDisplayObject>(this, callback));
                observedStore = observedObject;
                observedStoreProperty = observedProperty;
            }
            break;
        }
            
        case HSSParserNodeTypeExpression:
        {
            HSSExpression::p expressionValue = boost::static_pointer_cast<HSSExpression>(value);
            expressionValue->setPercentageBase(percentageBase);
            expressionValue->setPercentageObserved(observedProperty, observedObject);
            expressionValue->setScope(scope);
            ret = expressionValue->evaluate();
            if(callback != NULL){
                expressionValue->observe(HSSObservablePropertyValue, observedSourceProperty, this, new HSSValueChangedCallback<HSSDisplayObject>(this, callback));
                observedStore = expressionValue.get();
                observedStoreProperty = HSSObservablePropertyValue;
            }
            
            break;
        }
            
        case HSSParserNodeTypeKeywordConstant:
            
            break;
            
        default:
            throw "unknown parser node type while setting dHeight";
            break;
    }
    
    return ret;
}


