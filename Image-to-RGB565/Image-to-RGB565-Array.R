# =============================================================================
# Image-to-RGB565-Array.R
# 
# R script to convert an image to a uint16_t RGB565 array
# 
# As output, a C-header file is created
# 
# Hague Nusseck @ electricidea
# v1.1 25.December.2020
#
# Windows command line call:
# "C:\Program Files\R\R-4.0.2\bin\Rscript.exe" Image-to-RGB565-Array.R
# 
# for details about using the imager library see:
# https://cran.r-project.org/web/packages/imager/vignettes/gettingstarted.html
# https://metacpan.org/pod/distribution/Imager/lib/Imager/Files.pod
#
# for details about RGB565 see:
# http://www.barth-dev.de/online/rgb565-color-picker/
# online RGB565 Color calculator:
# http://www.rinkydinkelectronics.com/calc_rgb565.php
# =============================================================================


# load library "imager"
# This call automatically loads and installs the required packages 
# if they are not already installed.
if (!require("imager")) {
  install.packages("imager", dependencies = TRUE)
  library(imager)
}

# ------------------------------------------------------------------------------

# open the "file open"-Dialog
ImageFileName <- file.choose()
# extract the file path
ImageFilePath <- dirname(ImageFileName)
# load image file
InputImage <- load.image(paste(ImageFilePath, basename(ImageFileName), sep="/"))

# display the loaded image file
# Note: If the script is started from the command line, then the image is 
# saved as a PDF in the same folder. Cool feature!
plot(InputImage)

# convert the image as data frame
ImageData <- as.data.frame(InputImage,wide="c") 
# c.1 = Red color scaled from 0.0 to 1.0
# c.2 = Green color scaled from 0.0 to 1.0
# c.3 = Blue color scaled from 0.0 to 1.0

# convert color values to RGB565
ImageData$RGB565 <- 
  bitwShiftL(bitwAnd(ImageData$c.1*0xFF, 0xF8), 8) + # Red   0b11111000 shl 8
  bitwShiftL(bitwAnd(ImageData$c.2*0xFF, 0xFC), 3) + # Green 0b11111100 shl 3
  bitwShiftR(bitwAnd(ImageData$c.3*0xFF, 0xF8), 3)   # Blue  0b11111000 shr 3
# prepare the string with leading "0x"
ImageData$RGB565str <- paste("0x", 
                             as.character.hexmode(ImageData$RGB565, 
                                                  width = 4, 
                                                  upper.case = TRUE), 
                             sep="")


# ------------------------------------------------------------------------------

# extract the image filename without extension
# this will be used as the name of the image
ImageName <- sub(pattern = "(.*)\\..*$", replacement = "\\1", basename(ImageFileName))

# replace each occurrence of dash and space characters with an underscore
ImageName <- gsub("-", "_", ImageName)
ImageName <- gsub(" ", "_", ImageName)

# build the output filename for the RGB565-Array
ArrayFileName <- paste(ImageFilePath, "/", ImageName, ".h", sep="")

# write the header information in the file
cat(file = ArrayFileName,
    "// ", basename(ArrayFileName), "\n",
    "// \n",
    "// uint16_t RGB565 image array\n",
    "// created by Image-to-RGB565-Array.R (R-Script)\n\n",
    "// Hague Nusseck @ electricidea\n",
    "// v1.1 25.December.2020\n",
    "// \n\n\n",
    "// original image:\n",
    "// ", basename(ImageFileName), "\n\n",
    "#define ", ImageName, "_WIDTH ", dim(InputImage)[1], "\n",
    "#define ", ImageName, "_HEIGHT ", dim(InputImage)[2], "\n\n",
    "// image array:\n", 
    "const uint16_t ", ImageName, "[", length(ImageData$RGB565str), "] = {\n", 
    sep = "", fill = FALSE, labels = NULL,
    append = FALSE)

# write the formatted RGB565 data to the output file
write.table(t(matrix(ImageData$RGB565str, 
                   nrow = dim(InputImage)[1],ncol = dim(InputImage)[2])), 
            ArrayFileName, append = TRUE, quote = FALSE, sep = ", ",
            eol = ",\n", na = "NA", dec = ".", row.names = FALSE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "")

# write the closing "}" to the file
cat(file = ArrayFileName, "};\n",
    sep = "", fill = FALSE, labels = NULL, append = TRUE)

# ------------------------------------------------------------------------------

