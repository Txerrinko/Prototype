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
 *      Last changed: 2012/05/25
 *      HSS version: 1.0
 *      Core version: 0.47
 *      Revision: 4
 *
 ********************************************************************/

#ifndef HSSSTRINGCONSTANT_H
#define HSSSTRINGCONSTANT_H

#include "HSSExpression.h"
#include <boost/shared_ptr.hpp>

namespace AXR {
    
    /**
     *  @brief Parser node that represents either a single or double quoted string.
     *
     *  Text literals that are encolsed by single quotes ' or double quotes " in HSS.
     *  This class provides storage for that text.
     */
    class HSSStringConstant : public HSSParserNode
    {
    public:
        typedef boost::shared_ptr<HSSStringConstant> p;
        
        /**
         *  Creates a new instance of a string constant node which holds the given value.
         *  @param value    A string containing the value for the constant.
         */
        HSSStringConstant(std::string value);
        
        /**
         *  Clones an instance of HSSStringConstant and gives a shared pointer of the
         *  newly instanciated object.
         *  @return A shared pointer to the new HSSStringConstant
         */
        p clone() const;
        
        /**
         *  Destructor for this class.
         */
        virtual ~HSSStringConstant();
        
        /**
         *  Setter for the value.
         *  @param newValue     A string containing the new value for this constant.
         */
        void setValue(std::string newValue);
        
        /**
         *  Getter for the value.
         *  @param newValue     A string containing the new value for this constant.
         */
        std::string getValue();
        
        //see HSSParserNode.h for documentation of this method
        std::string toString();
        
    protected:
        std::string value;
    private:
        virtual HSSClonable::p cloneImpl() const;
    };
}



#endif

