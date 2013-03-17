package edu.gmu.vfml.classifier;

import static com.metsci.glimpse.util.logging.LoggerUtils.*;

import java.util.Enumeration;
import java.util.logging.Logger;

import weka.classifiers.Classifier;
import weka.core.Attribute;
import weka.core.Capabilities;
import weka.core.Capabilities.Capability;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.NoSupportForMissingValuesException;
import weka.core.TechnicalInformation;
import weka.core.TechnicalInformation.Field;
import weka.core.TechnicalInformation.Type;
import weka.core.TechnicalInformationHandler;
import weka.core.Utils;

/**
 * 
 * @see weka.classifiers.trees.Id3
 * @author ulman
 */
public class VFDT extends Classifier implements TechnicalInformationHandler
{
    private static final Logger logger = Logger.getLogger( VFDT.class.getName( ) );
    
    private static final long serialVersionUID = 1L;

    /**
     * <p>VFDT does not store entire training instances, only sufficient statistics
     * necessary to calculate Hoeffding bound and decide when to split nodes
     * and on which attributes to make the split.</p>
     * 
     * <p>Counts stores per-Node count values (in lieu of storing the entire set
     * of instances used at each Node.</p>
     */
    private class Node
    {
        /** The node's successors. */
        public Node[] successors;

        /** Attribute used for splitting. */
        public Attribute attribute;

        /** Class value if node is leaf. */
        public double classValue;

        /** The count of the class corresponding to classValue. */
        public int classCount;

        int[] counts;
        int[] classCounts;
        int totalCount;

        public Node( )
        {
            counts = new int[numClasses * sumAttributeValues];
            classCounts = new int[numClasses];
        }

        public void incrementCounts( Instance instance )
        {
            for ( int i = 0; i < instance.numAttributes( ); i++ )
            {
                Attribute attribute = instance.attribute( i );
                incrementCount( instance, attribute );
            }

            // update classValue and classCount
            int instanceClassCount = getCount( classAttribute.index( ) );

            // if the count of the class we just added is greater than the current
            // largest count, it becomes the new classification for this node
            if ( instanceClassCount > classCount )
            {
                classCount = instanceClassCount;
                classValue = instance.value( classAttribute );
            }
        }

        /**
         * @return the total number of instances in this Node
         */
        public int getCount( )
        {
            return totalCount;
        }

        /**
         * @param classIndex the class to get counts for
         * @return the total number of instances for the provided class
         */
        public int getCount( int classIndex )
        {
            return classCounts[classIndex];
        }

        /**
         * @param attribute the attribute to get a count for
         * @param instance the instance to get attribute value from
         * @return the total number of instances with the provided class and attribute value
         */
        public int getCount( Attribute attribute, Instance instance )
        {
            int classValue = ( int ) instance.value( classAttribute );
            int attributeIndex = attribute.index( );
            int attributeValue = ( int ) instance.value( attribute );
            return getCount( classValue, attributeIndex, attributeValue );
        }

        /**
         * @param classIndex
         * @param attribute
         * @return the total number of instances with the provided class
         */
        public int getCount( Attribute attribute, int valueIndex )
        {
            int attributeIndex = attribute.index( );

            int count = 0;
            for ( int classIndex = 0; classIndex < numClasses; classIndex++ )
            {
                count += getCount( classIndex, attributeIndex, valueIndex );
            }

            return count;
        }

        public int getCount( int classIndex, Attribute attribute, int valueIndex )
        {
            return getCount( classIndex, attribute.index( ), valueIndex );
        }
        
        /**
         * @param classIndex
         * @param attributeIndex
         * @param valueIndex
         * @return the number of instances with the provided class and attribute value
         */
        public int getCount( int classIndex, int attributeIndex, int valueIndex )
        {
            int attributeStartIndex = cumSumAttributeValues[attributeIndex];
            return counts[classIndex * sumAttributeValues + attributeStartIndex + valueIndex];
        }

        private void incrementCount( Instance instance, Attribute attribute )
        {
            int classValue = ( int ) instance.value( classAttribute );
            int attributeIndex = attribute.index( );
            int attributeValue = ( int ) instance.value( attribute );
            incrementCount( classValue, attributeIndex, attributeValue );
        }

        private void incrementCount( int classIndex, int attributeIndex, int valueIndex )
        {
            int attributeStartIndex = cumSumAttributeValues[attributeIndex];
            counts[classIndex * sumAttributeValues + attributeStartIndex + valueIndex] += 1;
            classCounts[classIndex] += 1;
            totalCount += 1;
        }
    }

    /** Root node of classification tree. */
    private Node root;

    /** Class attribute of dataset. */
    private Attribute classAttribute;

    private int numClasses;
    private double R_squared; // log2( numClasses )^2 
    private int numAttributes;
    private int sumAttributeValues;
    // cumulative sum of number of attribute 
    private int[] cumSumAttributeValues;

    private double delta;
    private double ln_inv_delta; // ln( 1 / delta )

    /**
     * See equation (1) in "Mining High-Speed Data Streams." The Hoeffding Bound provides
     * a bound on the true mean of a random variable given n independent
     * observations of the random variable, with probability 1 - delta
     * (where delta is the confidence level returned by this method).
     * 
     * @return the Hoeffding Bound confidence level
     */
    public double getConfidenceLevel( )
    {
        return delta;
    }

    /**
     * @see #getConfidenceLevel( )
     * @param delta
     */
    public void setConfidenceLevel( double delta )
    {
        this.delta = delta;
        this.ln_inv_delta = Math.log( 1 / delta );
    }

    /**
     * Returns a string describing the classifier.
     * @return a description suitable for the GUI.
     */
    public String globalInfo( )
    {
        //@formatter:off
        return "Class for constructing an unpruned decision tree based on the VFDT " +
               "algorithm. For more information see: \n\n" +
               getTechnicalInformation( ).toString( );
        //@formatter:on
    }

    /**
     * Returns an instance of a TechnicalInformation object, containing 
     * detailed information about the technical background of this class,
     * e.g., paper reference or book this class is based on.
     * 
     * @return the technical information about this class
     */
    public TechnicalInformation getTechnicalInformation( )
    {
        TechnicalInformation info = new TechnicalInformation( Type.ARTICLE );

        info.setValue( Field.AUTHOR, "Domingos, Pedro" );
        info.setValue( Field.YEAR, "2000" );
        info.setValue( Field.TITLE, "Mining high-speed data streams" );
        info.setValue( Field.JOURNAL, "Proceedings of the sixth ACM SIGKDD international conference on Knowledge discovery and data mining" );
        info.setValue( Field.SERIES, "KDD '00" );
        info.setValue( Field.ISBN, "1-58113-233-6" );
        info.setValue( Field.LOCATION, "Boston, Massachusetts, USA" );
        info.setValue( Field.PAGES, "71-80" );
        info.setValue( Field.URL, "http://doi.acm.org/10.1145/347090.347107" );
        info.setValue( Field.PUBLISHER, "ACM" );

        return info;
    }

    /**
     * Returns default capabilities of the classifier.
     *
     * @return the capabilities of this classifier
     */
    public Capabilities getCapabilities( )
    {
        Capabilities result = super.getCapabilities( );
        result.disableAll( );

        // attributes
        result.enable( Capability.NOMINAL_ATTRIBUTES );

        // class
        result.enable( Capability.NOMINAL_CLASS );
        result.enable( Capability.MISSING_CLASS_VALUES );

        // instances
        result.setMinimumNumberInstances( 0 );

        return result;
    }

    /**
     * Classifies a given test instance using the decision tree.
     *
     * @param instance the instance to be classified
     * @return the classification
     * @throws NoSupportForMissingValuesException if instance has missing values
     * @see weka.classifiers.trees.Id3#classifyInstance(Instance)
     */
    public double classifyInstance( Instance instance ) throws NoSupportForMissingValuesException
    {
        if ( instance.hasMissingValue( ) )
        {
            throw new NoSupportForMissingValuesException( "Id3: missing values not supported." );
        }

        // get the class value for the leaf node corresponding to the provided instance
        return getLeafNode( root, instance ).classValue;
    }

    /**
     * Builds Id3 decision tree classifier.
     *
     * @param data the training data
     * @exception Exception if classifier can't be built successfully
     */
    @Override
    public void buildClassifier( Instances data ) throws Exception
    {
        // can classifier handle the data?
        getCapabilities( ).testWithFail( data );

        // remove instances with missing class
        data = new Instances( data );
        data.deleteWithMissingClass( );

        // create root node
        root = new Node( );

        // store the class attribute for the data set
        classAttribute = data.classAttribute( );

        // record number of class values, attributes, and values for each attribute
        numClasses = data.classAttribute( ).numValues( );
        R_squared = Math.pow( Utils.log2( numClasses ), 2 );
        numAttributes = data.numAttributes( );
        cumSumAttributeValues = new int[numAttributes];
        sumAttributeValues = 0;
        for ( int i = 0; i < numAttributes; i++ )
        {
            Attribute attribute = data.attribute( i );
            cumSumAttributeValues[i] = sumAttributeValues;
            sumAttributeValues += attribute.numValues( );
        }

        // cap confidence level
        if ( delta <= 0 || delta > 1 )
        {
            setConfidenceLevel( 0.05 );
        }

        // build the Hoeffding tree
        makeTree( data );
    }

    /**
     * Method for building an Id3 tree.
     *
     * @param data the training data
     * @exception Exception if decision tree can't be built successfully
     */
    private void makeTree( Instances data ) throws Exception
    {
        makeTree( data.enumerateInstances( ) );
    }

    private void makeTree( Enumeration data )
    {
        while ( data.hasMoreElements( ) )
        {
            // retrieve the next data instance
            Instance instance = ( Instance ) data.nextElement( );

            // traverse the classification tree to find the leaf node for this instance
            Node node = getLeafNode( instance );

            // update the counts associated with this instance
            node.incrementCounts( instance );

            // determine based on Hoeffding Bound whether to split node
            
            int firstIndex = 0;
            int secondIndex = 0;
            double firstGain = 0.0;
            double secondGain = 0.0;
            
            try
            {
                // loop through all the attributes, calculating information gains
                // and keeping the attributes with the two highest information gains
                for ( int i = 0 ; i < instance.numAttributes( ) ; i++ )
                {
                    Attribute attribute = instance.attribute( i );
                    double gain = computeInfoGain( node, attribute );
                    
                    if ( gain > firstGain )
                    {
                        secondGain = firstGain;
                        secondIndex = firstIndex;
                        
                        firstGain = gain;
                        firstIndex = i;
                    }
                    else if ( gain > secondGain )
                    {
                        secondGain = gain;
                        secondIndex = i;
                    }
                }
            }
            catch ( Exception e )
            {
                logWarning( logger, "Trouble calculating information gain.", e );
            }
            finally
            {
                
            }
            
            // if the difference between the information gain of the two best attributes
            // has exceeded the Hoeffding bound (which will continually shrink as more
            // attributes are added to the node) then split on the best attribute 
            double hoeffdingBound = calculateHoeffdingBound( node );
            if ( firstGain - secondGain > hoeffdingBound )
            {
                
            }
        }
    }

    /**
     * Computes information gain for an attribute.
     *
     * @param data the data for which info gain is to be computed
     * @param att the attribute
     * @return the information gain for the given attribute and data
     * @throws Exception if computation fails
     * @see weka.classifiers.trees.Id3#computeInfoGain( Instances, Attribute )
     */
    private double computeInfoGain( Node node, Attribute attr ) throws Exception
    {
        double infoGain = computeEntropy( node );
        for ( int valueIndex = 0; valueIndex < attr.numValues( ); valueIndex++ )
        {
            int count = node.getCount( attr, valueIndex );
            
            if ( count > 0 )
            {
                double entropy = computeEntropy( node, attr, valueIndex );
                double ratio = ( ( double ) count / ( double ) node.getCount( ) ); 
                infoGain -= ratio * entropy;
            }
        }
        
        return infoGain;
    }

    /**
     * Computes the entropy of a dataset.
     * 
     * @param node the tree node for which entropy is to be computed
     * @return the entropy of the node's class distribution
     * @throws Exception if computation fails
     * @see weka.classifiers.trees.Id3#computeEntropy( Instances )
     */
    private double computeEntropy( Node node ) throws Exception
    {
        double entropy = 0;
        for ( int classIndex = 0; classIndex < numClasses; classIndex++ )
        {
            int classCount = node.getCount( classIndex );
            
            if ( classCount > 0 )
            {
                entropy -= classCount * Utils.log2( classCount );
            }
        }
        entropy /= ( double ) node.getCount( );
        return entropy + Utils.log2( node.getCount( ) );
    }

    /**
     * Computes the entropy of the child node created by splitting on the
     * provided attribute and value.
     * 
     * @param node the tree node for which entropy is to be computed
     * @param attr the attribute to split on before calculating entropy
     * @param valueIndex calculate entropy for the child node corresponding
     *        to this nominal attribute value index
     * @return calculated entropy
     * @throws Exception if computation fails
     */
    private double computeEntropy( Node node, Attribute attr, int valueIndex ) throws Exception
    {
        double entropy = 0;
        for ( int classIndex = 0; classIndex < numClasses; classIndex++ )
        {
            int count = node.getCount( classIndex, attr, valueIndex );

            if ( count > 0 )
            {
                entropy -= count * Utils.log2( count );
            }
        }
        
        int count = node.getCount( attr, valueIndex );
        entropy /= ( double ) count;
        return entropy + Utils.log2( count );
    }

    /**
     * Calculates the difference in information gain, epsilon, between the 
     * attribute with the best and second best information gain necessary to
     * make a splitting decision based on the current set of observed attributes.
     * As more attributes are gathered, the required difference will decrease.
     * 
     * @param node
     * @return
     */
    private double calculateHoeffdingBound( Node node )
    {
        int n = node.getCount( );
        double epsilon = Math.sqrt( ( R_squared * ln_inv_delta ) / ( 2 * n ) );
        return epsilon;
    }

    /**
     * @see #getLeafNode(Node, Instance)
     */
    private Node getLeafNode( Instance instance )
    {
        return getLeafNode( root, instance );
    }

    /**
     * Traverses the node tree for the provided instance and returns the leaf node
     * associated with the provided instance.
     * 
     * @param instance the instance to be classified
     * @return the leaf node for the instance
     * @see weka.classifiers.trees.Id3#classifyInstance(Instance)
     */
    private Node getLeafNode( Node node, Instance instance )
    {
        // this is a leaf node, so return this node
        if ( node.attribute == null )
        {
            return node;
        }
        // this is an internal node, move to the next child based on the m_Attribute for this node
        else
        {
            int attributeValue = ( int ) instance.value( node.attribute );
            Node childNode = node.successors[attributeValue];
            return getLeafNode( childNode, instance );
        }
    }
}
