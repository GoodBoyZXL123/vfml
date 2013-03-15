package edu.gmu.vfml.classifier;

import java.util.Enumeration;

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

/**
 * 
 * @see weka.classifiers.trees.Id3
 * @author ulman
 */
public class VFML extends Classifier implements TechnicalInformationHandler
{
    private static final long serialVersionUID = 1L;

    private class Node
    {
        /** The node's successors. */
        public Node[] successors;

        /** Attribute used for splitting. */
        public Attribute attribute;

        /** Class value if node is leaf. */
        public double classValue;
        
        /** instance counts at node per class, per attribute, per value */
        public Counts counts;
    }
    
    private class Counts
    {
        int[] counts;
        
        public Counts( Instances instances )
        {
            counts = new int[ numClasses * sumAttributeValues ];
        }
        
        public int getCount( Instance instance, Attribute attribute )
        {
            int classValue = (int) instance.value( classAttribute );
            int attributeIndex = attribute.index( );
            int attributeValue = (int) instance.value( attribute );
            return getCount( classValue, attributeIndex, attributeValue );
        }
        
        public int getCount( int classIndex, int attributeIndex, int valueIndex )
        {
            int attributeStartIndex = cumSumAttributeValues[attributeIndex];
            return counts[ classIndex * sumAttributeValues + attributeStartIndex + valueIndex ];
        }
        
        public void incrementCount( Instance instance, Attribute attribute )
        {
            int classValue = (int) instance.value( classAttribute );
            int attributeIndex = attribute.index( );
            int attributeValue = (int) instance.value( attribute );
            incrementCount( classValue, attributeIndex, attributeValue );
        }
        
        public void incrementCount( int classIndex, int attributeIndex, int valueIndex )
        {
            int attributeStartIndex = cumSumAttributeValues[attributeIndex];
            counts[ classIndex * sumAttributeValues + attributeStartIndex + valueIndex ] += 1;
        }
        
        public void incrementCounts( Instance instance )
        {
            for ( int i = 0 ; i < instance.numAttributes( ) ; i++ )
            {
                Attribute attribute = instance.attribute( i );
                incrementCount( instance, attribute );
            }
        }
    }

    /** Root node of classification tree. */
    private Node root;

    /** Class attribute of dataset. */
    private Attribute classAttribute;
    
    int numClasses;
    int numAttributes;
    int sumAttributeValues;
    int[] cumSumAttributeValues;

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
        
        // store the class attribute for the data set
        classAttribute = data.classAttribute( );
        
        // record number of class values, attributes, and values for each attribute
        numClasses = data.classAttribute( ).numValues( );
        numAttributes = data.numAttributes( );
        cumSumAttributeValues = new int[ numAttributes ];
        sumAttributeValues = 0;
        for ( int i = 0 ; i < numAttributes ; i++ )
        {
            Attribute attribute = data.attribute( i );
            cumSumAttributeValues[i] = sumAttributeValues;
            sumAttributeValues += attribute.numValues( );
        }

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
            Instance instance = ( Instance ) data.nextElement( );
        }
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
