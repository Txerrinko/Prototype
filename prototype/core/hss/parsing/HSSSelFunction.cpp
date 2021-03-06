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
 *      Last changed: 2012/06/22
 *      HSS version: 1.0
 *      Core version: 0.47
 *      Revision: 5
 *
 ********************************************************************/

#import "HSSSelFunction.h"
#import "../../axr/AXRController.h"

using namespace AXR;

HSSSelFunction::HSSSelFunction()
: HSSFunction()
{
    this->functionType = HSSFunctionTypeSel;
}

HSSSelFunction::~HSSSelFunction()
{
    
}

HSSSelFunction::HSSSelFunction(const HSSSelFunction & orig)
: HSSFunction(orig)
{
    
}

HSSSelFunction::p HSSSelFunction::clone() const
{
    return boost::static_pointer_cast<HSSSelFunction, HSSClonable>(this->cloneImpl());
}

std::string HSSSelFunction::toString()
{    
    std::string tempstr = std::string("HSSSelFunction\n");
    return tempstr;
}

const std::vector<HSSSelectorChain::p> & HSSSelFunction::getSelectorChains() const
{
    return this->selectorChains;
}

void HSSSelFunction::setSelectorChains(std::vector<HSSSelectorChain::p> newValues)
{
    this->selectorChains = newValues;
    this->setDirty(true);
}

void HSSSelFunction::selectorChainsAdd(HSSSelectorChain::p & newSelectorChain)
{
    if(newSelectorChain)
    {
        std_log3("Added selector chain to HSSFlagFunction: " << newSelectorChain->toString());
        newSelectorChain->setParentNode(this->shared_from_this());
        this->selectorChains.push_back(newSelectorChain);
    }
}

void HSSSelFunction::selectorChainsRemove(unsigned int index)
{
    this->selectorChains.erase(this->selectorChains.begin()+index);
}

void HSSSelFunction::selectorChainsRemoveLast()
{
    this->selectorChains.pop_back();
}

HSSSelectorChain::p & HSSSelFunction::selectorChainsGet(unsigned index)
{
    return this->selectorChains[index];
}

HSSSelectorChain::p & HSSSelFunction::selectorChainsLast()
{
    return this->selectorChains.back();
}

const int HSSSelFunction::selectorChainsSize()
{
    return this->selectorChains.size();
}

void * HSSSelFunction::_evaluate()
{
    this->selection = this->axrController->select(this->selectorChains, *this->scope, this->getThisObj());
    this->_value = (void*) &this->selection;
    return this->_value;
}

void * HSSSelFunction::_evaluate(std::deque<HSSParserNode::p> arguments)
{
    return this->_evaluate();
}

//void HSSSelFunction::valueChanged(HSSObservableProperty source, void*data)
//{
//    std_log1("######################### valueChanged");
//    this->setDirty(true);
//    this->_value = data;
//    this->notifyObservers(HSSObservablePropertyValue, this->_value);
//}


HSSClonable::p HSSSelFunction::cloneImpl() const{
    
    HSSSelFunction::p clone = HSSSelFunction::p(new HSSSelFunction(*this));
    
    HSSSelectorChain::const_it sIt;
    for (sIt=this->selectorChains.begin(); sIt!=this->selectorChains.end(); sIt++) {
        HSSSelectorChain::p clonedSelectorChain = (*sIt)->clone();
        clone->selectorChainsAdd(clonedSelectorChain);
    }    
    return clone;
}
