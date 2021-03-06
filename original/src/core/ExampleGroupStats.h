#ifndef EXAMPLEGROUPSTATSH
#define EXAMPLEGROUPSTATSH

#include "vfml.h"

/** \ingroup CoreAPI DecisionTree
*/

/** \file 

\brief Sufficient statistics for Entropy and Gini.

Tracks and maintains the sufficient statistics needed to calculate
Entropy and Gini of discrete and continuous attributes, as well as
make some queries about the probability of events in the data.

*/



typedef struct _Bin_ {
   float lowerBound, upperBound;
   float *classTotals;
   float exampleCount;
   int boundryClass;
   long boundryCount;
} Bin, *BinPtr;

BinPtr BinNew(ExampleSpecPtr spec);
void BinFree(BinPtr bin);
void BinWrite(BinPtr bin, FILE *out);

typedef struct _ContinuousTracker_ {
   ExampleSpecPtr spec;
   float *classTotals;
   long exampleCount;
   VoidListPtr bins;
   float min, max;
   int initMinMax;
   int timesPrunedCount;
   int addingNewBins;
} ContinuousTracker, *ContinuousTrackerPtr;

ContinuousTrackerPtr ContinuousTrackerNew(ExampleSpecPtr spec);
void ContinuousTrackerFree(ContinuousTrackerPtr ct);
void ContinuousTrackerAddExample(ContinuousTrackerPtr ct, 
                                    float value, int theclass);
float ContinuousTrackerGetPercentBelowThreshold(ContinuousTrackerPtr ct,
                                                      float thresh);

int ContinuousTrackerNumSplitThresholds(ContinuousTrackerPtr ct);
int ContinuousTrackerGetMostCommonClassInPartition(ContinuousTrackerPtr ct,
                        float threshold, int above);

void ContinuousTrackerEntropyAttributeSplit(ContinuousTrackerPtr ct,
                        float *firstIndex, float *firstThresh,
                        float *secondIndex, float *secondThresh);
void ContinuousTrackerGiniAttributeSplit(ContinuousTrackerPtr ct,
                        float *firstIndex, float *firstThresh,
                        float *secondIndex, float *secondThresh);

void ContinuousTrackerDisableWorseThanEntropy(ContinuousTrackerPtr ct, 
					      float entropyThresh);
void ContinuousTrackerDisableWorseThanGini(ContinuousTrackerPtr ct, 
					      float giniThresh);
int ContinuousTrackerPruneSplitsEntropy(ContinuousTrackerPtr ct,
                               int maxSplits, int pruneDownTo);

/** \brief Sufficient statistics for Entropy and Gini. */
typedef struct _ExampleGroupStats_ {
   ExampleSpecPtr spec;
   AttributeTrackerPtr attributeTracker;

   long examplesSeen;

   long *classTotals;

   /* VoidListPtr for each attribute:
        for discrete attributes another VoidListPtr for each value, and an
                            array of longs with the count for each class 
        for continuous attributes a continuous tracker structure
   */
   VoidAListPtr stats;

} ExampleGroupStats, *ExampleGroupStatsPtr;


/** \brief Creates a structure to track sufficient statistics.

Creates a structure to track the statistics needed to cacluate several
common machine metrics for the attributes that are active in the
AttributeTracker. This function takes over the memory for the
AttributeTracker and will free it when ExampleGroupStatsFree For
categorical attributes this uses memory proportional to the number of
classes * the number of values of the attribute. For continuous
attributes this uses constant memory at first, but as examples are
added with ExampleGroupStatsAddExample the memory grows proportionally
with the number of unique values of the attribute.
*/
ExampleGroupStatsPtr ExampleGroupStatsNew(ExampleSpecPtr es, 
                                       AttributeTrackerPtr at);

/** \brief Frees all the memory that was being used by the structure.*/
void ExampleGroupStatsFree(ExampleGroupStatsPtr egs);

/** \brief Temporarily frees the memory being used to hold statistics.

Does not free the whole structure. A later call to
ExampleGroupStatsReactivate will restore the memory (but not the
counts that used to be there). This is a convienient way to focus RAM
usage (and learning) in one part of the instance space while keeping
the book keeping around to quickly resume learning in another. You
shouldn't try to add examples to a deactiveated structure.
*/
void ExampleGroupStatsDeactivate(ExampleGroupStatsPtr egs);

/** \brief Reallocates the memory that is freed by a call to ExampleGroupStatsDeactivate. */
void ExampleGroupStatsReactivate(ExampleGroupStatsPtr egs);

/** \brief Adds the information from the example to the statistics structure. */
void ExampleGroupStatsAddExample(ExampleGroupStatsPtr egs, ExamplePtr e);

/** \brief A debugging function that prints a representation of the stats structure to specified file. */
void ExampleGroupStatsWrite(ExampleGroupStatsPtr egs, FILE *out);

/** \brief Number of examples being tracked by the structure 

Outputs the number of examples added to the structure with
ExampleGroupStatsAddExample since the last call to
ExampleGroupStatsReactivate.
*/
long ExampleGroupStatsNumExamplesSeen(ExampleGroupStatsPtr egs);


/** \brief Returns the attribute tracker associated with the structure. */
AttributeTrackerPtr ExampleGroupStatsGetAttributeTracker(
                                      ExampleGroupStatsPtr egs);

/** \brief Tests if the attribute is active.

Returns 1 if the attribute was active in the initial attribute tracker
and has not been ignored by a call to ExampleGroupStatsIgnoreAttribute
since then.
*/
int ExampleGroupStatsIsAttributeActive(ExampleGroupStatsPtr egs, int num);

/** \brief Frees the memory being used by the attribute and stops
tracking it. 

This is useful if you decide that some attribute will not be used
(perhaps using some statistical tests) and would like to use the
memory elsewhere.
*/
void ExampleGroupStatsIgnoreAttribute(ExampleGroupStatsPtr egs, int num);


/** \brief Returns the index of the most common class, but adds
addCount samples to addClass.

Use addClass of -1 for no addition (or just call
ExampleGroupStatsGetMostCommonClass). This adding allows you to, for
example, smooth the class towards the parent class during decision
tree induction.
*/
int ExampleGroupStatsGetMostCommonClassLaplace(ExampleGroupStatsPtr egs, 
					       int addClass, int addCount);
/** \brief Returns the index of the most common class. */
int ExampleGroupStatsGetMostCommonClass(ExampleGroupStatsPtr egs);

/** \brief Returns the number of examples with the most common class that were seen by the structure. */
long ExampleGroupStatsGetMostCommonClassCount(ExampleGroupStatsPtr egs);


/** \brief Returns the most common class among examples where the specified attribute has the specified value. 

Should only be called for discrete attributes.
*/
int ExampleGroupStatsGetMostCommonClassForAttVal(ExampleGroupStatsPtr egs,
                                                int att, int val);

/** \brief Returns 1 if all the examples shown to the structure have the same class. */
int ExampleGroupStatsIsPure(ExampleGroupStatsPtr egs);

/** \brief Returns the fraction of examples that have the specified value for the specified attribute. 

Should only be called for discrete attributes.
*/
float ExampleGroupStatsGetValuePercent(ExampleGroupStatsPtr egs, 
				       int attNum, int valNum);
float ExampleGroupStatsGetValueGivenClassPercent(ExampleGroupStatsPtr egs, 
                                        int attNum, int valNum, int classNum);

/** \brief Returns P(att = value | class).

Returns the fraction of examples among those that have the specified
class that have the specified value for the specified attribute, but
smooths the return value by adding a small amount (that decreases with
the number of samples seen) to each class count first.

Should only be called for discrete attributes.
*/
double ExampleGroupStatsGetValueGivenClassMEstimate(ExampleGroupStatsPtr egs, 
                                        int attNum, int valNum, int classNum);

/** \brief Returns P(class). */
float ExampleGroupStatsGetClassPercent(ExampleGroupStatsPtr egs, int classNum);

/** \brief Returns the fraction of examples with a value below the specified threshold. 

Should only be called for continuous attributes. 
*/
float ExampleGroupStatsGetPercentBelowThreshold(ExampleGroupStatsPtr egs,
                                     int attNum, float thresh);

/* for naive bayes */

/** \brief Returns a smoothed P(att = value | class).

Returns the log of the fraction of examples among those that have the
specified class that have the specified value for the specified
attribute, but smooths the return value by adding a small amount (that
decreases with the number of samples seen) to each class count first.

Should only be called for discrete attributes.
*/
double ExampleGroupStatsGetValueGivenClassMEstimateLogP(ExampleGroupStatsPtr 
                             egs, int attNum, int valNum, int classNum);

/** \brief Returns the log of the fraction of examples that have the specified class. */
double ExampleGroupStatsGetClassLogP(ExampleGroupStatsPtr egs, int classNum);


/** \brief Returns the entropy of the class attribute of all examples seen so far. */
float ExampleGroupStatsEntropyTotal(ExampleGroupStatsPtr egs);

/** \brief Returns the weighted entropy of the class attribute after partitoning the data by the values of the specified attribute. 

Should only be called for discrete attributes.
*/
float ExampleGroupStatsEntropyDiscreteAttributeSplit(ExampleGroupStatsPtr egs,
                                                           int attNum);

/** \brief Returns upper bound on weighted entropy of the class attribute after partitoning the data by the values of the specified attribute. 

This uses the Hoeffding bound and the empirical probabilities to
return a value that is higher than the true entropy with probability 1
- delta.

Should only be called for discrete attributes.
*/
float ExampleGroupStatsEntropyPlusDiscreteAttributeSplit(ExampleGroupStatsPtr 
                                 egs, int attNum, float delta);


/** \brief Returns lower bound on weighted entropy of the class attribute after partitoning the data by the values of the specified attribute. 

This uses the Hoeffding bound and the empirical probabilities to
return a value that is lower than the true entropy with probability 1
- delta.

Should only be called for discrete attributes.
*/
float ExampleGroupStatsEntropyMinusDiscreteAttributeSplit(ExampleGroupStatsPtr 
                                 egs, int attNum, float delta);


/** \brief Finds the entropy of the best split thresholds.

Calculates the entropy of splitting the specified attribute by every
threshold under consideration (values are sorted and then a threshold
is considered between each pair of adjacent values that have different
class). The remaining arguments return the entropy of the best and
second best thresholds, along with the thresholds themselves.

This function adds an MDL penelty similar to the one Quinlan uses in C4.5.

Should only be called for continuous attributes. 
*/
void ExampleGroupStatsEntropyContinuousAttributeSplit(ExampleGroupStatsPtr egs, 
                 int attNum, float *firstIndex, float *firstThresh,
                             float *secondIndex, float *secondThresh);

/** \brief Returns the gini index of the class attribute of all examples seen so far. */
float ExampleGroupStatsGiniTotal(ExampleGroupStatsPtr egs);

/** \brief Returns the weighted gini of the class attribute after
partitoning the data by the values of the specified attribute.

Should only be called for discrete attributes.
*/
float ExampleGroupStatsGiniDiscreteAttributeSplit(ExampleGroupStatsPtr egs, 
                                                      int attNum);

/** \brief Finds the Gini index of the best split thresholds.

Calculates the gini of splitting the specified attribute by
every threshold under consideration (values are sorted and then a
threshold is considered between each pair of adjacent values that have
different class). The remaining arguments return the gini of the best
and second best thresholds, along with the thresholds themselves.

Should only be called for continuous attributes. 
*/
void ExampleGroupStatsGiniContinuousAttributeSplit(ExampleGroupStatsPtr egs, 
                 int attNum, float *firstIndex, float *firstThresh,
                             float *secondIndex, float *secondThresh);

/** \brief Stop monitoring some thresholds.

Stop monitoring every threshold with an entropy worse than the
specified value. This frees some memory, but adding future values to
the egs may require some interpolation to estimate the position of the
new value in the array of all values for the attribute (and so this
introduces some error into future calls for the Entropy or Gini of the
attribute).

Should only be called for continuous attributes. 
*/
void ExampleGroupStatsIgnoreSplitsWorseThanEntropy(ExampleGroupStatsPtr egs,
				   int attNum, float entropyThresh);

/** \brief Stop monitoring some thresholds.

Stop monitoring every threshold with an Gini worse than the specified
value. This frees some memory, but adding future values to the egs may
require some interpolation to estimate the position of the new value
in the array of all values for the attribute (and so this introduces
some error into future calls for the Entropy or Gini of the
attribute).

Should only be called for continuous attributes. 
*/
void ExampleGroupStatsIgnoreSplitsWorseThanGini(ExampleGroupStatsPtr egs,
						int attNum, float giniThresh);


/** \brief Reduce the number of thresholds being considered if above the max

If the attribute is monitoring more than 'maxSplits' split thresholds
this function will find the best 'pruneDownTo' based on entropy and
start ignoring all the rest. This frees some memory, but adding future
values to the egs may require some interpolation to estimate the
position of the new value in the array of all values for the attribute
(and so this introduces some error into future calls for the Entropy
or Gini of the attribute).

Returns the number of thresholds that were pruned. 

Should only be called for continuous attributes. 
*/
int ExampleGroupStatsLimitSplitsEntropy(ExampleGroupStatsPtr egs, int attNum,
                               int maxSplits, int pruneDownTo);

/** \brief Stop adding new split thresholds, but continue to use future examples to evaluate the existing ones. 

Should only be called for continuous attributes. 
*/
void ExampleGroupStatsStopAddingSplits(ExampleGroupStatsPtr egs, int attNum);

/** \brief Returns the number of thresholds that are being monitored for the specified attribute. 

Should only be called for continuous attributes. 
*/
int ExampleGroupStatsNumSplitThresholds(ExampleGroupStatsPtr egs, int attNum);

/** \brief Returns the most common class above the specified value.

Should only be called for continuous attributes. 
*/
int ExampleGroupStatsGetMostCommonClassAboveThreshold(ExampleGroupStatsPtr egs,
                      int attNum, float threshold);

/** \brief Returns the most common class below the specified value.

Should only be called for continuous attributes. 
*/int ExampleGroupStatsGetMostCommonClassBelowThreshold(ExampleGroupStatsPtr egs,
                      int attNum, float threshold);

/* undocumented, used by CVFDT  */
void ExampleGroupStatsRemoveExample(ExampleGroupStatsPtr egs, ExamplePtr e);

#endif /* EXAMPLEGROUPSTATSH */
