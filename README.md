# CrossCutter
B&amp;R AutomationStudio project implementing a CrossCutter for E-Camp Spring 2025.

# Input Parameter Limits
For this project, there were several design decisions that were made to limit the possible inputs. These are explained below.
## Velocity
In Automatic Mode, the conveyor speed is limited to a maximum speed of 130 products / minute, which is about 150 mm/s. This was the maximum possible speed while avoiding lag error during operation or while stopping.

In Manual Mode, the conveyor is limited to a speed of 1,340 mm/s, while the cutter is limited to a speed of 3,400 deg/s. These were experimentally determined to be the fastest possible speeds while avoiding a lag error.
## Recipe
For the synchronized zone recipe, several limits were imposed on the new recipe creation:
- The Degrees Before and Degrees After need to match. This is due to the fact that the cam automat is set up from center to center, therefore if they were unequal, it could cause some unpredictable behavior during compensation. Plus, for a cut through a product, it is desirable to have synchronization before and after to an equal degree to avoid damaging the product.
- In a similar vein, the Conveyor Distance needs to be the sum of the Degrees Before and Degrees After. This is to ensure that the motion with be 1:1 during the cam (1 mm/s : 1 deg/s)
## Print Mark
There were limits imposed on the inputs to the Print Mark as well.
- Expectation Window: the limits for the width of the expectation window were set to 15mm-50mm. Any larger than 50mm and there is a risk of picking up the invalid print mark, and any smaller than 15mm and the valid print marks may be missed.
- Print Mark Width: The maximum and minimum input values were limited at reasonable values to ensure that the print mark could still be read.