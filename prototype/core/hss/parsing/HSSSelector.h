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
 *      Last changed: 2012/06/04
 *      HSS version: 1.0
 *      Core version: 0.47
 *      Revision: 6
 *
 ********************************************************************/

#ifndef HSSSELECTOR_H
#define HSSSELECTOR_H

#include "HSSParserNode.h"
#include <string>
#include <boost/shared_ptr.hpp>

namespace AXR {
    
    /**
     *  @brief Selects elements by tag name.
     *
     *  This just holds information about the name of the elements that should
     *  be selected by AXRController when creating selections.
     */
    class HSSSelector : public HSSParserNode {
    public:
        typedef boost::shared_ptr<HSSSelector> p;
        
        /**
         *  Creates a new instance of a selector.
         */
        HSSSelector(std::string elementName);
        
        /**
         *  Copy constructor for HSSAction objects. Do not call directly, use clone() instead.
         */
        HSSSelector(const HSSSelector &orig);
        
        /**
         *  Clones an instance of HSSSelector and gives a shared pointer of the
         *  newly instanciated object.
         *  @return A shared pointer to the new HSSSelector.
         */
        p clone() const;
        
        /**
         *  Getter for the name of the element.
         *  @return A string containing the name of the element.
         */
        std::string getElementName();
        
        //see HSSObject.h for documentation of this method
        virtual std::string toString();
        
        
    protected:
        std::string elementName;
        
    private:
        virtual HSSClonable::p cloneImpl() const;
    };
}



#endif
