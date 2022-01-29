module.exports = (temp, product) => {
  let output = temp.replace(/{%PRODUCTNAME%}/g, product.productName);
  output = output.replace(/{%IMAGE%}/g, product.image);
  output = output.replace(/{%PRICE%}/g, product.ratio);
  output = output.replace(/{%NUTRIENTS%}/g, product.saftey);
  output = output.replace(/{%QUANTITY%}/g, product.quantity);
  output = output.replace(/{%DESCRIPTION%}/g, product.description);
  output = output.replace(/{%FROM%}/g, product.address);
  output = output.replace(/{%ID%}/g, product.id);

  if (!product.lowPopulation)
    output = output.replace(/{%NOT_ORGANIC%}/g, "high-traffic");
  return output;
};
